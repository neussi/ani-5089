// =============================================================================
// Chapitre 04 - Exercice 27 : Cent cubes, et le premier mur du réel
// cubes.cpp : Mesure du coût du draw-call individuel vs rendu instancié groupé
// UE-ANI-IA-5049 — Coder pour la VR, XR et AR III
// =============================================================================

#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>

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
    d.appName = "Ch04Exo27Cubes";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig winCfg;
    winCfg.title = "Exo 27 — Cent cubes et instanciation";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;
    winCfg.resizable = false;

    NkWindow window(winCfg);
    if (!window.IsValid()) return 1;

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = static_cast<uint32>(window.GetSize().width);
    devInfo.height = static_cast<uint32>(window.GetSize().height);

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) {
        window.Close();
        return 2;
    }

    NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        if (renderer) NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 3;
    }

    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshHandle meshCube = renderer->GetMeshSystem()->GetCube();
    uint32 W = devInfo.width;
    uint32 H = devInfo.height;

    auto setupScene = [&]() {
        NkCamera3DData camData;
        camData.up = {0.0f, 1.0f, 0.0f};
        camData.fovY = 60.0f;
        camData.aspect = static_cast<float32>(W) / static_cast<float32>(H);
        camData.nearPlane = 0.05f;
        camData.farPlane = 100.0f;

        NkCamera3D camera(camData);
        camera.SetPosition({0.0f, 2.50f, 3.5f});
        camera.SetTarget({0.0f, 0.0f, 0.0f});

        NkSceneContext sctx;
        sctx.camera = camera;
        sctx.time = 0.0f;

        NkLightDesc sun;
        sun.type = NkLightType::NK_DIRECTIONAL;
        sun.direction = {-0.4f, -0.8f, -0.4f};
        sun.color = {1.0f, 0.98f, 0.95f};
        sun.intensity = 2.6f;
        sun.castShadow = true;
        sctx.lights.PushBack(sun);

        r3d->BeginScene(sctx);
    };

    std::cout << "--- Mesure du rendu un par un (One by one) ---" << std::endl;
    const int nbCubesOneByOne = 100;
    const int nbFrames = 60;

    auto t0 = std::chrono::high_resolution_clock::now();
    for (int frame = 0; frame < nbFrames; ++frame) {
        if (renderer->BeginFrame()) {
            setupScene();
            for (int i = 0; i < nbCubesOneByOne; ++i) {
                float32 x = (i % 10) * 0.25f - 1.125f;
                float32 z = (i / 10) * 0.25f - 1.125f;
                NkDrawCall3D dc;
                dc.mesh = meshCube;
                dc.transform = NkMat4f::Translate({x, 0.05f, z}) * NkMat4f::Scale({0.10f, 0.10f, 0.10f});
                dc.aabb = {
                    {x - 0.05f, 0.0f, z - 0.05f},
                    {x + 0.05f, 0.10f, z + 0.05f}
                };
                dc.tint = {0.8f, 0.8f, 0.8f};
                dc.roughness = 0.7f;
                dc.castShadow = false;
                r3d->Submit(dc);
            }
            renderer->Present();
            renderer->EndFrame();
        }
    }
    device->WaitIdle();
    auto t1 = std::chrono::high_resolution_clock::now();
    double fpsOneByOne = static_cast<double>(nbFrames) / std::chrono::duration<double>(t1 - t0).count();
    std::cout << "One by one: " << nbCubesOneByOne << " cubes, " << nbCubesOneByOne
              << " drawCalls, " << std::fixed << std::setprecision(1) << fpsOneByOne << " fps" << std::endl;

    std::cout << "--- Mesure du rendu instancie (Grouped) ---" << std::endl;
    const int nbCubesGrouped = 800;
    NkDrawCallInstanced instDc;
    instDc.mesh = meshCube;
    instDc.aabb = {{-3.0f, 0.0f, -3.0f}, {3.0f, 0.5f, 3.0f}};

    for (int i = 0; i < nbCubesGrouped; ++i) {
        float32 x = (i % 40) * 0.12f - 2.4f;
        float32 z = (i / 40) * 0.12f - 2.4f;
        instDc.transforms.PushBack(NkMat4f::Translate({x, 0.035f, z}) * NkMat4f::Scale({0.05f, 0.05f, 0.05f}));
        instDc.tints.PushBack({0.75f, 0.80f, 0.85f});
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    for (int frame = 0; frame < nbFrames; ++frame) {
        if (renderer->BeginFrame()) {
            setupScene();
            r3d->SubmitInstanced(instDc);
            renderer->Present();
            renderer->EndFrame();
        }
    }
    device->WaitIdle();
    auto t3 = std::chrono::high_resolution_clock::now();
    double fpsGrouped = static_cast<double>(nbFrames) / std::chrono::duration<double>(t3 - t2).count();
    std::cout << "Grouped: 1 drawCalls, " << std::fixed << std::setprecision(1) << fpsGrouped
              << " fps, " << nbCubesGrouped << " cubes" << std::endl;

    // Écriture de cadence.txt
    std::ofstream out("cadence.txt");
    if (out.is_open()) {
        out << "One by one: " << nbCubesOneByOne << " cubes, " << nbCubesOneByOne
            << " drawCalls, " << std::fixed << std::setprecision(1) << fpsOneByOne << " fps\n\n";
        out << "Grouped: 1 drawCalls, " << std::fixed << std::setprecision(1) << fpsGrouped
            << " fps, " << nbCubesGrouped << " cubes\n\n";
        out << "Found:\n";
        out << "La structure de dessin instancie NkDrawCallInstanced et la methode SubmitInstanced,\n";
        out << "declarees dans les fichiers d'en-tete NKRendererTypes.h et NkRender3D.h.\n";
        out.close();
    }

    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();

    return 0;
}
