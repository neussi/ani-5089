// =============================================================================
// Chapitre 05 - Exercice 4 : Le premier mur texturé
// UE-ANI-IA-5049 — Coder pour la VR, XR et AR III
// =============================================================================

#include <iostream>
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
#include "NKRenderer/Materials/NkMaterial.h"
#include "Commun/NkTutoCapture.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "Ch05Exo04Mur";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
    (void)state;

    // Détermine si on affiche avec texture (apres) ou sans texture (avant)
    const char *texEnv = getenv("APPLIQUER_TEXTURE");
    bool appliquerTexture = (texEnv == nullptr || strcmp(texEnv, "0") != 0);

    NkWindowConfig winCfg;
    winCfg.title = "Exo 4 — Le premier mur texture";
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

    // Chargement de la texture UNIQUE une seule fois avant la boucle
    NkTexHandle murTex = renderer->GetTextures()->Load("murs/beton.png");
    NkMaterial *matMur = NkMaterial::Create(renderer->GetMaterials(), NkMaterialType::NK_PBR_METALLIC);
    if (matMur) {
        matMur->SetAlbedoMap(murTex);
        matMur->SetTriplanarTileSize(1.0f);
    }

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
        // Même point de vue pour avant et apres
        camera.SetPosition({0.0f, 1.40f, 1.80f});
        camera.SetTarget({0.0f, 1.25f, -2.0f});

        NkSceneContext sctx;
        sctx.camera = camera;
        sctx.time = totalTime;

        NkLightDesc sun;
        sun.type = NkLightType::NK_DIRECTIONAL;
        sun.direction = {-0.45f, -0.80f, -0.40f};
        sun.color = {1.0f, 0.98f, 0.94f};
        sun.intensity = 2.6f;
        sun.castShadow = true;
        sctx.lights.PushBack(sun);

        r3d->BeginScene(sctx);

        auto Pave = [&](NkVec3f center, NkVec3f size, NkVec3f tint, NkMaterial *mat = nullptr) {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            dc.transform = NkMat4f::Translate(center) * NkMat4f::Scale(size);
            dc.aabb = {
                {center.x - size.x * 0.5f, center.y - size.y * 0.5f, center.z - size.z * 0.5f},
                {center.x + size.x * 0.5f, center.y + size.y * 0.5f, center.z + size.z * 0.5f}
            };
            if (mat) {
                dc.material = mat->GetInstHandle();
            }
            dc.tint = tint;
            dc.roughness = 0.7f;
            dc.castShadow = true;
            r3d->Submit(dc);
        };

        // Sol
        Pave({0.0f, -0.05f, 0.0f}, {4.0f, 0.10f, 4.0f}, {0.35f, 0.35f, 0.38f});

        // Mur du fond : SEUL ce mur reçoit le matériau texturé
        if (appliquerTexture && matMur) {
            Pave({0.0f, 1.25f, -2.0f}, {4.0f, 2.50f, 0.10f}, {1.0f, 1.0f, 1.0f}, matMur);
        } else {
            Pave({0.0f, 1.25f, -2.0f}, {4.0f, 2.50f, 0.10f}, {0.70f, 0.72f, 0.75f});
        }

        // Mur gauche : reste brut comme avant
        Pave({-2.0f, 1.25f, 0.0f}, {0.10f, 2.50f, 4.0f}, {0.68f, 0.70f, 0.72f});

        // Mur droit : reste brut comme avant
        Pave({2.0f, 1.25f, 0.0f}, {0.10f, 2.50f, 4.0f}, {0.68f, 0.70f, 0.72f});

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
