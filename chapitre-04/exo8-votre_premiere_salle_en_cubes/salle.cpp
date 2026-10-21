// =============================================================================
// Exercice 8 : Votre première salle en cubes
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

#include <fstream>
#include <iostream>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "Exo08Salle";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
    (void)state;

    // 1. Initialisation de la fenêtre native
    NkWindowConfig winCfg;
    winCfg.title = "Exo 8 — Votre premiere salle en cubes";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;
    winCfg.resizable = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[Exo08] Impossible de creer la fenetre");
        return 1;
    }

    // 2. Initialisation du device GPU (RHI auto-détecté : OpenGL / Vulkan)
    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) {
        logger.Error("[Exo08] Initialisation du device GPU impossible");
        window.Close();
        return 2;
    }

    const char *backendName = NkGraphicsApiName(device->GetApi());
    logger.Info("[Exo08] Backend actif : {0}", backendName);

    // Enregistrement du backend réel dans backend.txt
    {
        std::ofstream backendFile("backend.txt");
        if (backendFile.is_open()) {
            backendFile << backendName << std::endl;
        }
    }

    // 3. Création du Renderer
    NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        logger.Error("[Exo08] Echec initialisation renderer");
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 3;
    }

    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshHandle meshCube = renderer->GetMeshSystem()->GetCube();
    NkMeshHandle meshPlane = renderer->GetMeshSystem()->GetPlane();

    // 4. Gestion des événements
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

    // 5. Boucle principale de rendu
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

        // Configuration de la caméra à l'intérieur de la salle
        NkCamera3DData camData;
        camData.up = {0.0f, 1.0f, 0.0f};
        camData.fovY = 65.0f;
        camData.aspect = (float32)W / (float32)H;
        camData.nearPlane = 0.05f;
        camData.farPlane = 100.0f;

        NkCamera3D camera(camData);
        // Position légèrement surélevée et reculée pour avoir une vue d'ensemble de la salle
        camera.SetPosition({0.0f, 1.40f, 1.60f});
        camera.SetTarget({0.0f, 1.25f, -1.0f});

        NkSceneContext sctx;
        sctx.camera = camera;
        sctx.time = totalTime;

        // Lumière directionnelle pour éclairer l'intérieur de la salle
        NkLightDesc sun;
        sun.type = NkLightType::NK_DIRECTIONAL;
        sun.direction = {-0.3f, -0.9f, -0.4f};
        sun.color = {1.0f, 0.95f, 0.88f};
        sun.intensity = 2.8f;
        sun.castShadow = true;
        sctx.lights.PushBack(sun);

        r3d->BeginScene(sctx);

        // Définition géométrique de la salle : 4m x 4m, hauteur 2.50m.
        // Ordre strict : Translate puis Scale (T * S)

        // 1. Sol (y = 0)
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

        // 2. Plafond (y = 2.50m)
        {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            dc.transform = NkMat4f::Translate({0.0f, 2.55f, 0.0f}) * NkMat4f::Scale({4.0f, 0.10f, 4.0f});
            dc.aabb = {{-2.0f, 2.50f, -2.0f}, {2.0f, 2.60f, 2.0f}};
            dc.tint = {0.80f, 0.80f, 0.82f};
            dc.roughness = 0.9f;
            r3d->Submit(dc);
        }

        // 3. Mur du fond (z = -2.0m)
        {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            dc.transform = NkMat4f::Translate({0.0f, 1.25f, -2.0f}) * NkMat4f::Scale({4.0f, 2.50f, 0.10f});
            dc.aabb = {{-2.0f, 0.0f, -2.05f}, {2.0f, 2.50f, -1.95f}};
            dc.tint = {0.65f, 0.68f, 0.72f};
            dc.roughness = 0.7f;
            r3d->Submit(dc);
        }

        // 4. Mur de face / entrée (z = +2.0m)
        {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            dc.transform = NkMat4f::Translate({0.0f, 1.25f, 2.0f}) * NkMat4f::Scale({4.0f, 2.50f, 0.10f});
            dc.aabb = {{-2.0f, 0.0f, 1.95f}, {2.0f, 2.50f, 2.05f}};
            dc.tint = {0.65f, 0.68f, 0.72f};
            dc.roughness = 0.7f;
            r3d->Submit(dc);
        }

        // 5. Mur gauche (x = -2.0m)
        {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            dc.transform = NkMat4f::Translate({-2.0f, 1.25f, 0.0f}) * NkMat4f::Scale({0.10f, 2.50f, 4.0f});
            dc.aabb = {{-2.05f, 0.0f, -2.0f}, {-1.95f, 2.50f, 2.0f}};
            dc.tint = {0.55f, 0.58f, 0.62f};
            dc.roughness = 0.7f;
            r3d->Submit(dc);
        }

        // 6. Mur droit (x = +2.0m)
        {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            dc.transform = NkMat4f::Translate({2.0f, 1.25f, 0.0f}) * NkMat4f::Scale({0.10f, 2.50f, 4.0f});
            dc.aabb = {{1.95f, 0.0f, -2.0f}, {2.05f, 2.50f, 2.0f}};
            dc.tint = {0.55f, 0.58f, 0.62f};
            dc.roughness = 0.7f;
            r3d->Submit(dc);
        }

        // Cube repère au centre de la pièce (1m devant la vue, posé sur le sol)
        {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            dc.transform = NkMat4f::Translate({0.0f, 0.25f, 0.0f}) * NkMat4f::Scale({0.5f, 0.5f, 0.5f});
            dc.aabb = {{-0.25f, 0.0f, -0.25f}, {0.25f, 0.50f, 0.25f}};
            dc.tint = {0.90f, 0.45f, 0.20f};
            dc.roughness = 0.3f;
            dc.metallic = 0.2f;
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
