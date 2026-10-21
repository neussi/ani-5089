// =============================================================================
// Exercice 25 : La lumière qui change tout
// UE-ANI-IA-5049 — Coder pour la VR, XR et AR III
// Implémentation réelle avec le moteur Nkentseu (NKWindow, NKRHI, NKRenderer)
// =============================================================================

#include "NKPlatform/NkPlatformDetect.h"
#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKTime/NkTime.h"
#include "NKLogger/NkLog.h"

#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"
#include "Commun/NkTutoCapture.h"
#include <cstring>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "Exo25Lumiere";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

struct ObjetPose {
    NkMat4f transform;
    NkAABB aabb;
};

inline ObjetPose PoserAuSol(NkVec3f taille, float32 x, float32 z) {
    float32 centreY = taille.y * 0.5f;
    NkVec3f c = {x, centreY, z};
    ObjetPose res;
    res.transform = NkMat4f::Translate(c) * NkMat4f::Scale(taille);
    res.aabb = {
        {c.x - taille.x * 0.5f, c.y - taille.y * 0.5f, c.z - taille.z * 0.5f},
        {c.x + taille.x * 0.5f, c.y + taille.y * 0.5f, c.z + taille.z * 0.5f}
    };
    return res;
}

inline ObjetPose PoserSur(float32 hauteurBase, NkVec3f taille, float32 x, float32 z) {
    float32 centreY = hauteurBase + (taille.y * 0.5f);
    NkVec3f c = {x, centreY, z};
    ObjetPose res;
    res.transform = NkMat4f::Translate(c) * NkMat4f::Scale(taille);
    res.aabb = {
        {c.x - taille.x * 0.5f, centreY - taille.y * 0.5f, c.z - taille.z * 0.5f},
        {c.x + taille.x * 0.5f, centreY + taille.y * 0.5f, c.z + taille.z * 0.5f}
    };
    return res;
}

int nkmain(const NkEntryState &state) {
    (void)state;

    // Détermination du mode d'éclairage via variable d'environnement ou argument
    // Modes : "depart" (défaut), "direction", "intensite", "ombres"
    const char *modeEnv = getenv("SCENE_MODE");
    NkString mode = (modeEnv && *modeEnv) ? modeEnv : "depart";

    NkLightDesc sun;
    sun.type = NkLightType::NK_DIRECTIONAL;
    sun.color = {1.0f, 0.95f, 0.88f};

    if (mode == "direction") {
        // Variation de direction seule : lumière rasante
        sun.direction = {0.85f, -0.20f, -0.25f};
        sun.intensity = 3.0f;
        sun.castShadow = true;
    } else if (mode == "intensite") {
        // Variation d'intensité seule : lumière sous-exposée
        sun.direction = {-0.40f, -1.00f, -0.30f};
        sun.intensity = 0.6f;
        sun.castShadow = true;
    } else if (mode == "ombres") {
        // Sans ombres portées
        sun.direction = {-0.40f, -1.00f, -0.30f};
        sun.intensity = 3.0f;
        sun.castShadow = false;
    } else {
        // Mode départ par défaut
        sun.direction = {-0.40f, -1.00f, -0.30f};
        sun.intensity = 3.0f;
        sun.castShadow = true;
    }

    NkWindowConfig winCfg;
    winCfg.title = "Exo 25 — La lumiere qui change tout";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;
    winCfg.resizable = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) return 1;

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) {
        window.Close();
        return 2;
    }

    NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 3;
    }

    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshHandle meshCube = renderer->GetMeshSystem()->GetCube();

    bool running = true;
    uint32 W = devInfo.width;
    uint32 H = devInfo.height;
    NkEventSystem &events = NkEvents();

    events.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) { running = false; });
    events.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() == NkKey::NK_ESCAPE) running = false;
    });
    events.AddEventCallback<NkWindowResizeEvent>([&](NkWindowResizeEvent *e) {
        uint32 w = (uint32)e->GetWidth(), h = (uint32)e->GetHeight();
        if (w > 0 && h > 0 && (w != W || h != H)) {
            W = w; H = h;
            renderer->OnResize(w, h);
        }
    });

    NkClock clock;
    float32 totalTime = 0.0f;
    tuto::NkTutoCapture capture;

    const float32 tableX = 0.0f;
    const float32 tableZ = 0.0f;
    const NkVec3f dimPlateau = {1.40f, 0.05f, 0.80f};
    const float32 hauteurTable = 0.75f;
    const float32 hauteurPieds = hauteurTable - dimPlateau.y;
    const NkVec3f dimPied = {0.06f, hauteurPieds, 0.06f};

    const float32 legOffsetX = (dimPlateau.x * 0.5f) - (dimPied.x * 0.5f) - 0.03f;
    const float32 legOffsetZ = (dimPlateau.z * 0.5f) - (dimPied.z * 0.5f) - 0.03f;

    while (running && window.IsOpen()) {
        events.PollEvents();
        if (!running) break;
        if (capture.Etape(renderer, device, W, H)) break;

        float32 dt = clock.Tick().delta;
        totalTime = capture.Temps(totalTime + dt);

        if (!renderer->BeginFrame()) continue;

        NkCamera3DData camData;
        camData.up = {0.0f, 1.0f, 0.0f};
        camData.fovY = 60.0f;
        camData.aspect = (float32)W / (float32)H;
        camData.nearPlane = 0.05f;
        camData.farPlane = 100.0f;

        NkCamera3D camera(camData);
        camera.SetPosition({0.60f, 1.70f, 2.40f});
        camera.SetTarget({0.0f, 0.60f, 0.0f});

        NkSceneContext sctx;
        sctx.camera = camera;
        sctx.time = totalTime;
        sctx.lights.PushBack(sun);

        r3d->BeginScene(sctx);

        auto submitPose = [&](const ObjetPose &op, NkVec3f tint, float32 roughness, bool shadow = true) {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            dc.transform = op.transform;
            dc.aabb = op.aabb;
            dc.tint = tint;
            dc.roughness = roughness;
            dc.castShadow = shadow;
            r3d->Submit(dc);
        };

        // Sol (4x4m)
        {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            dc.transform = NkMat4f::Translate({0.0f, -0.05f, 0.0f}) * NkMat4f::Scale({4.0f, 0.10f, 4.0f});
            dc.aabb = {{-2.0f, -0.10f, -2.0f}, {2.0f, 0.0f, 2.0f}};
            dc.tint = {0.40f, 0.40f, 0.42f};
            dc.roughness = 0.8f;
            dc.castShadow = false;
            r3d->Submit(dc);
        }

        // Murs
        submitPose(PoserAuSol({4.0f, 2.50f, 0.10f}, 0.0f, -2.0f), {0.75f, 0.77f, 0.80f}, 0.7f, true);
        submitPose(PoserAuSol({0.10f, 2.50f, 4.0f}, -2.0f, 0.0f), {0.73f, 0.75f, 0.78f}, 0.7f, true);

        // Table (pieds + plateau)
        submitPose(PoserAuSol(dimPied, tableX - legOffsetX, tableZ - legOffsetZ), {0.26f, 0.18f, 0.10f}, 0.6f, true);
        submitPose(PoserAuSol(dimPied, tableX + legOffsetX, tableZ - legOffsetZ), {0.26f, 0.18f, 0.10f}, 0.6f, true);
        submitPose(PoserAuSol(dimPied, tableX - legOffsetX, tableZ + legOffsetZ), {0.26f, 0.18f, 0.10f}, 0.6f, true);
        submitPose(PoserAuSol(dimPied, tableX + legOffsetX, tableZ + legOffsetZ), {0.26f, 0.18f, 0.10f}, 0.6f, true);
        submitPose(PoserSur(hauteurPieds, dimPlateau, tableX, tableZ), {0.68f, 0.46f, 0.28f}, 0.5f, true);

        // Objets sur la table
        submitPose(PoserSur(hauteurTable, {0.20f, 0.15f, 0.20f}, tableX - 0.25f, tableZ), {0.20f, 0.45f, 0.70f}, 0.4f, true);
        submitPose(PoserSur(hauteurTable, {0.28f, 0.05f, 0.20f}, tableX + 0.25f, tableZ + 0.05f), {0.80f, 0.28f, 0.20f}, 0.5f, true);

        renderer->Present();
        renderer->EndFrame();
        capture.ImageRendue();
    }

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return capture.CodeDeSortie(0);
}
