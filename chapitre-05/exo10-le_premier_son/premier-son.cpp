// =============================================================================
// Chapitre 05 - Exercice 10 : Le premier son
// UE-ANI-IA-5049 — Coder pour la VR, XR et AR III
// =============================================================================

#include <iostream>
#include <fstream>
#include "NKPlatform/NkPlatformDetect.h"
#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKTime/NkTime.h"
#include "NKLogger/NkLog.h"

#include "NKAudio/NkAudio.h"

#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"
#include "Commun/NkTutoCapture.h"

using namespace nkentseu;
using namespace nkentseu::audio;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "Ch05Exo10Son";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
    (void)state;

    // 1. Initialisation du moteur audio
    AudioEngineConfig audioCfg;
    audioCfg.sampleRate = 48000;
    AudioEngine::Instance().Initialize(audioCfg);

    // 2. Génération et lecture d'un son (sinusoïde 440 Hz pendant 1 seconde)
    AudioSample son = AudioGenerator::GenerateTone(440.0f, 1.0f);
    AudioEngine::Instance().Play(son);

    // 3. Affichage des quatre paramètres demandés
    std::cout << "dorsal : " << AudioEngine::Instance().GetBackendName() << "\n";
    std::cout << "frequence : " << AudioEngine::Instance().GetSampleRate() << "\n";
    std::cout << "tampon : " << AudioEngine::Instance().GetBufferSize() << "\n";
    std::cout << "latence : " << AudioEngine::Instance().GetLatencyMs() << std::endl;

    // Écriture dans demarrage.txt
    std::ofstream out("demarrage.txt");
    if (out.is_open()) {
        out << "dorsal : " << AudioEngine::Instance().GetBackendName() << "\n";
        out << "frequence : " << AudioEngine::Instance().GetSampleRate() << "\n";
        out << "tampon : " << AudioEngine::Instance().GetBufferSize() << "\n";
        out << "latence : " << AudioEngine::Instance().GetLatencyMs() << "\n";
        out.close();
    }

    // 4. Fenêtre et rendu de la pièce
    NkWindowConfig winCfg;
    winCfg.title = "Exo 10 — Le premier son";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;
    winCfg.resizable = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        AudioEngine::Instance().Shutdown();
        AudioLoader::Free(son);
        return 1;
    }

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) {
        window.Close();
        AudioEngine::Instance().Shutdown();
        AudioLoader::Free(son);
        return 2;
    }

    NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        AudioEngine::Instance().Shutdown();
        AudioLoader::Free(son);
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

        // Sol et murs
        drawBox({0.0f, -0.05f, 0.0f}, {4.0f, 0.10f, 4.0f}, {0.35f, 0.35f, 0.38f});
        drawBox({0.0f, 1.25f, -2.00f}, {4.0f, 2.50f, 0.10f}, {0.70f, 0.72f, 0.75f});
        drawBox({-2.0f, 1.25f, 0.0f}, {0.10f, 2.50f, 4.0f}, {0.68f, 0.70f, 0.72f});

        renderer->Present();
        renderer->EndFrame();
        capture.ImageRendue();
    }

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();

    // 5. Arrêt du moteur audio et libération du sample
    AudioEngine::Instance().Shutdown();
    AudioLoader::Free(son);

    return capture.CodeDeSortie(0);
}
