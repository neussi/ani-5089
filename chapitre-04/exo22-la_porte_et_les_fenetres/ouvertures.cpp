// =============================================================================
// Exercice 22 : La porte et les fenêtres
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
    d.appName = "Exo22Ouvertures";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
    (void)state;

    // 1. Fenêtre
    NkWindowConfig winCfg;
    winCfg.title = "Exo 22 — La porte et les fenetres";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;
    winCfg.resizable = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) return 1;

    // 2. Device GPU
    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) {
        window.Close();
        return 2;
    }

    // 3. Renderer
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

    // Décalage pour éliminer le Z-fighting (5 mm vers l'intérieur)
    const float32 offset = 0.005f;

    while (running && window.IsOpen()) {
        events.PollEvents();
        if (!running) break;
        if (capture.Etape(renderer, device, W, H)) break;

        float32 dt = clock.Tick().delta;
        totalTime = capture.Temps(totalTime + dt);

        if (!renderer->BeginFrame()) continue;

        // Caméra orientée vers le mur du fond et le mur gauche pour voir la porte et les fenêtres
        NkCamera3DData camData;
        camData.up = {0.0f, 1.0f, 0.0f};
        camData.fovY = 65.0f;
        camData.aspect = (float32)W / (float32)H;
        camData.nearPlane = 0.05f;
        camData.farPlane = 100.0f;

        NkCamera3D camera(camData);
        camera.SetPosition({1.20f, 1.45f, 1.30f});
        camera.SetTarget({-0.60f, 1.20f, -1.50f});

        NkSceneContext sctx;
        sctx.camera = camera;
        sctx.time = totalTime;

        NkLightDesc sun;
        sun.type = NkLightType::NK_DIRECTIONAL;
        sun.direction = {-0.4f, -0.8f, -0.5f};
        sun.color = {1.0f, 0.95f, 0.85f};
        sun.intensity = 2.8f;
        sun.castShadow = true;
        sctx.lights.PushBack(sun);

        r3d->BeginScene(sctx);

        // --- 1. Salle (Sol, Plafond, Murs) ---
        // Sol
        {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            dc.transform = NkMat4f::Translate({0.0f, -0.05f, 0.0f}) * NkMat4f::Scale({4.0f, 0.10f, 4.0f});
            dc.aabb = {{-2.0f, -0.10f, -2.0f}, {2.0f, 0.0f, 2.0f}};
            dc.tint = {0.35f, 0.35f, 0.38f};
            dc.roughness = 0.8f;
            dc.castShadow = false;
            r3d->Submit(dc);
        }
        // Plafond
        {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            dc.transform = NkMat4f::Translate({0.0f, 2.55f, 0.0f}) * NkMat4f::Scale({4.0f, 0.10f, 4.0f});
            dc.aabb = {{-2.0f, 2.50f, -2.0f}, {2.0f, 2.60f, 2.0f}};
            dc.tint = {0.80f, 0.80f, 0.82f};
            dc.roughness = 0.9f;
            r3d->Submit(dc);
        }
        // Mur fond (z = -2.0m)
        {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            dc.transform = NkMat4f::Translate({0.0f, 1.25f, -2.0f}) * NkMat4f::Scale({4.0f, 2.50f, 0.10f});
            dc.aabb = {{-2.0f, 0.0f, -2.05f}, {2.0f, 2.50f, -1.95f}};
            dc.tint = {0.70f, 0.72f, 0.75f};
            dc.roughness = 0.7f;
            r3d->Submit(dc);
        }
        // Mur gauche (x = -2.0m)
        {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            dc.transform = NkMat4f::Translate({-2.0f, 1.25f, 0.0f}) * NkMat4f::Scale({0.10f, 2.50f, 4.0f});
            dc.aabb = {{-2.05f, 0.0f, -2.0f}, {-1.95f, 2.50f, 2.0f}};
            dc.tint = {0.60f, 0.63f, 0.67f};
            dc.roughness = 0.7f;
            r3d->Submit(dc);
        }

        // --- 2. Porte (posée sur mur fond à z = -2.0 + offset) ---
        // Largeur = 0.90m, Hauteur = 2.00m, Épaisseur = 0.02m
        // Posée au sol (centre y = 1.00m), x = -0.70m
        {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            float px = -0.70f, py = 1.00f, pz = -2.00f + offset;
            dc.transform = NkMat4f::Translate({px, py, pz}) * NkMat4f::Scale({0.90f, 2.00f, 0.02f});
            dc.aabb = {{px - 0.45f, 0.0f, pz - 0.01f}, {px + 0.45f, 2.0f, pz + 0.01f}};
            dc.tint = {0.45f, 0.25f, 0.12f}; // Teinte bois chêne
            dc.roughness = 0.5f;
            dc.castShadow = true;
            r3d->Submit(dc);
        }

        // --- 3. Deux Fenêtres (sur mur gauche à x = -2.0 + offset) ---
        // Largeur = 1.20m (axe z), Hauteur = 1.00m (axe y), Épaisseur = 0.02m (axe x)
        // Allège à 0.90m -> centre y = 0.90 + 0.50 = 1.40m
        // Fenêtre 1 (z = -0.70m)
        {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            float fx = -2.00f + offset, fy = 1.40f, fz = -0.70f;
            dc.transform = NkMat4f::Translate({fx, fy, fz}) * NkMat4f::Scale({0.02f, 1.00f, 1.20f});
            dc.aabb = {{fx - 0.01f, fy - 0.50f, fz - 0.60f}, {fx + 0.01f, fy + 0.50f, fz + 0.60f}};
            dc.tint = {0.65f, 0.82f, 0.92f}; // Vitrage bleuté
            dc.roughness = 0.2f;
            dc.metallic = 0.1f;
            dc.castShadow = true;
            r3d->Submit(dc);
        }
        // Fenêtre 2 (z = +0.70m)
        {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            float fx = -2.00f + offset, fy = 1.40f, fz = +0.70f;
            dc.transform = NkMat4f::Translate({fx, fy, fz}) * NkMat4f::Scale({0.02f, 1.00f, 1.20f});
            dc.aabb = {{fx - 0.01f, fy - 0.50f, fz - 0.60f}, {fx + 0.01f, fy + 0.50f, fz + 0.60f}};
            dc.tint = {0.65f, 0.82f, 0.92f};
            dc.roughness = 0.2f;
            dc.metallic = 0.1f;
            dc.castShadow = true;
            r3d->Submit(dc);
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
