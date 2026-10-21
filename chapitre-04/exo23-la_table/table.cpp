// =============================================================================
// Exercice 23 : La table
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

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "Exo23Table";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig winCfg;
    winCfg.title = "Exo 23 — La table";
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

    // Dimensions table
    const float32 tableX = 0.0f;
    const float32 tableZ = 0.0f;
    const float32 plateauW = 1.40f;   // Longueur X
    const float32 plateauD = 0.80f;   // Profondeur Z
    const float32 plateauT = 0.05f;   // Épaisseur Y (5 cm)
    const float32 tableTopY = 0.75f;  // Face supérieure à 0.75 m du sol
    const float32 plateauCenterY = tableTopY - (plateauT * 0.5f); // 0.725 m

    const float32 legSection = 0.06f; // 6 cm x 6 cm
    const float32 legH = tableTopY - plateauT; // 0.70 m
    const float32 legCenterY = legH * 0.5f;     // 0.35 m

    const float32 legOffsetX = (plateauW * 0.5f) - (legSection * 0.5f) - 0.03f;
    const float32 legOffsetZ = (plateauD * 0.5f) - (legSection * 0.5f) - 0.03f;

    NkVec3f legPositions[4] = {
        {tableX - legOffsetX, legCenterY, tableZ - legOffsetZ},
        {tableX + legOffsetX, legCenterY, tableZ - legOffsetZ},
        {tableX - legOffsetX, legCenterY, tableZ + legOffsetZ},
        {tableX + legOffsetX, legCenterY, tableZ + legOffsetZ}
    };

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
        camera.SetPosition({0.0f, 1.55f, 2.30f});
        camera.SetTarget({0.0f, 0.65f, 0.0f});

        NkSceneContext sctx;
        sctx.camera = camera;
        sctx.time = totalTime;

        NkLightDesc sun;
        sun.type = NkLightType::NK_DIRECTIONAL;
        sun.direction = {-0.45f, -0.85f, -0.30f};
        sun.color = {1.0f, 0.96f, 0.90f};
        sun.intensity = 2.6f;
        sun.castShadow = true;
        sctx.lights.PushBack(sun);

        r3d->BeginScene(sctx);

        auto drawBox = [&](NkVec3f center, NkVec3f size, NkVec3f tint, float32 roughness, bool castShadow = true) {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            dc.transform = NkMat4f::Translate(center) * NkMat4f::Scale(size);
            dc.aabb = {
                {center.x - size.x * 0.5f, center.y - size.y * 0.5f, center.z - size.z * 0.5f},
                {center.x + size.x * 0.5f, center.y + size.y * 0.5f, center.z + size.z * 0.5f}
            };
            dc.tint = tint;
            dc.roughness = roughness;
            dc.castShadow = castShadow;
            r3d->Submit(dc);
        };

        // Sol (4x4m)
        drawBox({0.0f, -0.05f, 0.0f}, {4.0f, 0.10f, 4.0f}, {0.35f, 0.35f, 0.38f}, 0.8f, false);
        // Mur fond (z = -2.0m)
        drawBox({0.0f, 1.25f, -2.0f}, {4.0f, 2.50f, 0.10f}, {0.70f, 0.72f, 0.75f}, 0.7f, true);
        // Mur gauche (x = -2.0m)
        drawBox({-2.0f, 1.25f, 0.0f}, {0.10f, 2.50f, 4.0f}, {0.68f, 0.70f, 0.73f}, 0.7f, true);

        // Plateau de la table (bois naturel chaleureux)
        drawBox({tableX, plateauCenterY, tableZ}, {plateauW, plateauT, plateauD}, {0.65f, 0.42f, 0.25f}, 0.5f, true);

        // Les 4 pieds (bois teinté plus sombre)
        for (int i = 0; i < 4; ++i) {
            drawBox(legPositions[i], {legSection, legH, legSection}, {0.28f, 0.18f, 0.10f}, 0.6f, true);
        }

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
