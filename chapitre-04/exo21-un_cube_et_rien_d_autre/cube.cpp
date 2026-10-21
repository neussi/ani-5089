// =============================================================================
// Exercice 21 : Un cube, et rien d'autre
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
    d.appName = "Exo21Cube";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
    (void)state;

    // 1. Creation de la fenetre
    NkWindowConfig winCfg;
    winCfg.title = "Exo 21 — Un cube, et rien d'autre";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;
    winCfg.resizable = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) return 1;

    // 2. Creation du device GPU
    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) {
        window.Close();
        return 2;
    }

    // 3. Creation du renderer
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
    NkMeshHandle meshPlane = renderer->GetMeshSystem()->GetPlane();

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

        // Camera a l'entree : position (0.0, 1.70, 1.5), cible (0.0, 1.40, -2.0)
        NkCamera3DData camData;
        camData.up = {0.0f, 1.0f, 0.0f};
        camData.fovY = 70.0f;
        camData.aspect = (float32)W / (float32)H;
        camData.nearPlane = 0.05f;
        camData.farPlane = 200.0f;

        NkCamera3D camera(camData);
        NkVec3f camPos = {0.0f, 1.70f, 1.50f};
        NkVec3f camTarget = {0.0f, 1.40f, -2.00f};
        camera.SetPosition(camPos);
        camera.SetTarget(camTarget);

        NkSceneContext sctx;
        sctx.camera = camera;
        sctx.time = totalTime;

        // Soleil (lumiere directionnelle)
        NkLightDesc sun;
        sun.type = NkLightType::NK_DIRECTIONAL;
        sun.direction = {-0.4f, -1.0f, -0.3f};
        sun.color = {1.0f, 0.95f, 0.85f};
        sun.intensity = 3.0f;
        sun.castShadow = true;
        sctx.lights.PushBack(sun);

        r3d->BeginScene(sctx);

        // Sol pour recevoir l'ombre du cube
        {
            NkDrawCall3D dc;
            dc.mesh = meshPlane;
            dc.transform = NkMat4f::Scale({10.0f, 1.0f, 10.0f});
            dc.aabb = {{-10.0f, -0.01f, -10.0f}, {10.0f, 0.01f, 10.0f}};
            dc.tint = {0.25f, 0.25f, 0.28f};
            dc.roughness = 0.9f;
            dc.castShadow = false;
            r3d->Submit(dc);
        }

        // Preparation du cube a deux metres devant la camera
        NkVec3f forward = (camTarget - camPos).Normalized();
        NkVec3f cubePos = camPos + forward * 2.0f;

        NkDrawCall3D cubeDrawCall;
        cubeDrawCall.mesh = meshCube;
        cubeDrawCall.transform = NkMat4f::Translate(cubePos) * NkMat4f::Scale({1.0f, 1.0f, 1.0f});
        cubeDrawCall.tint = {1.0f, 1.0f, 1.0f};
        cubeDrawCall.roughness = 0.4f;
        cubeDrawCall.metallic = 0.1f;
        cubeDrawCall.castShadow = true;

        // Boîte englobante AABB monde (cubePos +- 0.5) pour eviter que le frustum culling ne le rejette
        cubeDrawCall.aabb = {
            {cubePos.x - 0.5f, cubePos.y - 0.5f, cubePos.z - 0.5f},
            {cubePos.x + 0.5f, cubePos.y + 0.5f, cubePos.z + 0.5f}
        };

        r3d->Submit(cubeDrawCall);

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
