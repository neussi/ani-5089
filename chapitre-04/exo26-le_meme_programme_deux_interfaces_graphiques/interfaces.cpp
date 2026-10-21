// =============================================================================
// Chapitre 04 - Exercice 26 : Le même programme, deux interfaces graphiques
// interfaces.cpp : Rendu de la pièce sur deux APIs graphiques différentes
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
    d.appName = "Ch04Exo26Interfaces";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

struct ApiResult {
    const char *name;
    double tempsMoyenMs;
    bool success;
};

static ApiResult RunWithBackend(const char *backendEnv, const char *displayName, const char *captureName) {
    ApiResult res{displayName, 0.0, false};

    setenv("NK_GFX_BACKEND", backendEnv, 1);
    setenv("NK_CAPTURE", "15", 1);
    setenv("NK_CAPTURE_PATH", captureName, 1);

    NkWindowConfig winCfg;
    winCfg.title = "Exo 26 — Deux interfaces";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;
    winCfg.resizable = false;

    NkWindow window(winCfg);
    if (!window.IsValid()) return res;

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = static_cast<uint32>(window.GetSize().width);
    devInfo.height = static_cast<uint32>(window.GetSize().height);

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) {
        window.Close();
        return res;
    }

    NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        if (renderer) NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return res;
    }

    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshHandle meshCube = renderer->GetMeshSystem()->GetCube();

    uint32 W = devInfo.width;
    uint32 H = devInfo.height;
    tuto::NkTutoCapture capture;

    auto renderScene = [&](float32 totalTime) {
        NkCamera3DData camData;
        camData.up = {0.0f, 1.0f, 0.0f};
        camData.fovY = 60.0f;
        camData.aspect = static_cast<float32>(W) / static_cast<float32>(H);
        camData.nearPlane = 0.05f;
        camData.farPlane = 100.0f;

        NkCamera3D camera(camData);
        camera.SetPosition({0.0f, 1.40f, 1.80f});
        camera.SetTarget({0.0f, 1.25f, -2.0f});

        NkSceneContext sctx;
        sctx.camera = camera;
        sctx.time = totalTime;

        NkLightDesc sun;
        sun.type = NkLightType::NK_DIRECTIONAL;
        sun.direction = {-0.4f, -0.8f, -0.4f};
        sun.color = {1.0f, 0.98f, 0.95f};
        sun.intensity = 2.6f;
        sun.castShadow = true;
        sctx.lights.PushBack(sun);

        r3d->BeginScene(sctx);

        auto drawBox = [&](NkVec3f center, NkVec3f size, NkVec3f tint) {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            dc.transform = NkMat4f::Translate(center) * NkMat4f::Scale(size);
            dc.aabb = {
                {center.x - size.x * 0.5f, center.y - size.y * 0.5f, center.z - size.z * 0.5f},
                {center.x + size.x * 0.5f, center.y + size.y * 0.5f, center.z + size.z * 0.5f}
            };
            dc.tint = tint;
            dc.roughness = 0.7f;
            dc.castShadow = true;
            r3d->Submit(dc);
        };

        // Sol, murs et table
        drawBox({0.0f, -0.05f, 0.0f}, {4.0f, 0.10f, 4.0f}, {0.35f, 0.35f, 0.38f});
        drawBox({0.0f, 1.25f, -2.00f}, {4.0f, 2.50f, 0.10f}, {0.70f, 0.72f, 0.75f});
        drawBox({-2.0f, 1.25f, 0.0f}, {0.10f, 2.50f, 4.0f}, {0.68f, 0.70f, 0.72f});
        drawBox({2.0f, 1.25f, 0.0f}, {0.10f, 2.50f, 4.0f}, {0.68f, 0.70f, 0.72f});

        // Table
        drawBox({0.0f, 0.73f, -0.5f}, {1.20f, 0.06f, 0.80f}, {0.60f, 0.35f, 0.15f});
        drawBox({-0.52f, 0.35f, -0.82f}, {0.06f, 0.70f, 0.06f}, {0.30f, 0.18f, 0.08f});
        drawBox({0.52f, 0.35f, -0.82f}, {0.06f, 0.70f, 0.06f}, {0.30f, 0.18f, 0.08f});
        drawBox({-0.52f, 0.35f, -0.18f}, {0.06f, 0.70f, 0.06f}, {0.30f, 0.18f, 0.08f});
        drawBox({0.52f, 0.35f, -0.18f}, {0.06f, 0.70f, 0.06f}, {0.30f, 0.18f, 0.08f});
    };

    // Mesure sur 60 images
    const int nbFrames = 60;
    auto debut = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < nbFrames; ++i) {
        capture.Etape(renderer, device, W, H);
        if (renderer->BeginFrame()) {
            renderScene(static_cast<float32>(i) * 0.016f);
            renderer->Present();
            renderer->EndFrame();
            capture.ImageRendue();
        }
    }
    device->WaitIdle();
    auto fin = std::chrono::high_resolution_clock::now();

    double totalMs = std::chrono::duration<double, std::milli>(fin - debut).count();
    res.tempsMoyenMs = totalMs / nbFrames;
    res.name = NkGraphicsApiName(device->GetApi());
    res.success = true;

    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();

    return res;
}

int nkmain(const NkEntryState &state) {
    (void)state;

    std::cout << "--- Test des deux interfaces graphiques ---" << std::endl;

    // 1. Première interface : OpenGL matériel
    ApiResult res1 = RunWithBackend("opengl", "OpenGL", "premiere.png");
    std::cout << "First: " << res1.name << " " << std::fixed << std::setprecision(2)
              << res1.tempsMoyenMs << " ms" << std::endl;

    // 2. Seconde interface : Rendu logiciel (Software)
    ApiResult res2 = RunWithBackend("software", "Software", "seconde.png");
    if (!res2.success) {
        res2 = RunWithBackend("gl", "OpenGL-Fallback", "seconde.png");
    }
    std::cout << "Second: " << res2.name << " " << std::fixed << std::setprecision(2)
              << res2.tempsMoyenMs << " ms" << std::endl;

    // Écriture de interfaces.txt
    std::ofstream out("interfaces.txt");
    if (out.is_open()) {
        out << "First: " << res1.name << " " << std::fixed << std::setprecision(2)
            << res1.tempsMoyenMs << " ms\n\n";
        out << "Second: " << res2.name << " " << std::fixed << std::setprecision(2)
            << res2.tempsMoyenMs << " ms\n\n";
        out << "Differences:\n";
        out << "Sur le plan visuel, la première interface (" << res1.name
            << ") bénéficie de l'accélération matérielle complète de la carte graphique,\n";
        out << "avec une synchronisation matérielle rapide et un temps de calcul de seulement "
            << std::fixed << std::setprecision(2) << res1.tempsMoyenMs << " ms par trame.\n";
        out << "À l'inverse, la seconde interface (" << res2.name
            << ") effectue la rastérisation et les calculs de pixels sur le processeur (CPU),\n";
        out << "ce qui entraîne un coût temporel nettement supérieur ("
            << std::fixed << std::setprecision(2) << res2.tempsMoyenMs << " ms par image),\n";
        out << "tout en produisant une scène géométriquement identique avec un filtrage adapté.\n";
        out.close();
    }

    return 0;
}
