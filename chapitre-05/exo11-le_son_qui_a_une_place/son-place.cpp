// =============================================================================
// Chapitre 05 - Exercice 11 : Le son qui a une place
// son-place.cpp : Son de pluie spatialisé à la fenêtre avec écouteur lié à la caméra
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
    d.appName = "Ch05Exo11SonPlace";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
    (void)state;

    // 1. Initialisation du moteur audio
    AudioEngineConfig audioCfg;
    audioCfg.sampleRate = 48000;
    AudioEngine::Instance().Initialize(audioCfg);

    // 2. Génération du son de pluie (bruit rose de 4 secondes)
    AudioSample pluie = AudioGenerator::GenerateNoise(4.0f, WaveformType::NOISE_PINK);

    // Paramètres de la source 3D
    VoiceParams p;
    p.looping = true;
    p.volume = 1.0f;
    p.source3d.positional = true;
    // Placement à la première fenêtre : (-1.98 ; 1.40 ; -0.90)
    p.source3d.position[0] = -1.98f;
    p.source3d.position[1] = 1.40f;
    p.source3d.position[2] = -0.90f;
    // Distances choisies : minDistance = 1.0 m, maxDistance = 15.0 m
    p.source3d.minDistance = 1.0f;
    p.source3d.maxDistance = 15.0f;
    p.source3d.rolloffFactor = 1.0f;
    p.source3d.attenuationModel = AttenuationModel::INVERSE;

    AudioHandle voixPluie = AudioEngine::Instance().Play(pluie, p);

    // 3. Initialisation de la fenêtre et du renderer 3D
    NkWindowConfig winCfg;
    winCfg.title = "Exo 11 — Le son qui a une place";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;
    winCfg.resizable = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        AudioEngine::Instance().Stop(voixPluie);
        AudioEngine::Instance().Shutdown();
        AudioLoader::Free(pluie);
        return 1;
    }

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) {
        window.Close();
        AudioEngine::Instance().Stop(voixPluie);
        AudioEngine::Instance().Shutdown();
        AudioLoader::Free(pluie);
        return 2;
    }

    NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        AudioEngine::Instance().Stop(voixPluie);
        AudioEngine::Instance().Shutdown();
        AudioLoader::Free(pluie);
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

    // Position initiale de la caméra
    NkVec3f camPos{0.0f, 1.40f, 1.80f};
    NkVec3f camTarget{0.0f, 1.25f, -2.0f};
    NkVec3f camUp{0.0f, 1.0f, 0.0f};

    // Configuration du listener au démarrage
    NkVec3f camFwd = camTarget - camPos;
    camFwd.Normalize();
    AudioEngine::Instance().SetListenerPosition(camPos.x, camPos.y, camPos.z);
    AudioEngine::Instance().SetListenerOrientation(camFwd.x, camFwd.y, camFwd.z, camUp.x, camUp.y, camUp.z);

    NkClock clock;
    float32 totalTime = 0.0f;
    tuto::NkTutoCapture capture;

    while (running && window.IsOpen()) {
        events.PollEvents();
        if (!running) break;
        if (capture.Etape(renderer, device, W, H)) break;

        float32 dt = clock.Tick().delta;
        totalTime = capture.Temps(totalTime + dt);

        // Mise à jour de la position et de l'orientation de l'auditeur pour suivre la caméra
        AudioEngine::Instance().SetListenerPosition(camPos.x, camPos.y, camPos.z);
        AudioEngine::Instance().SetListenerOrientation(camFwd.x, camFwd.y, camFwd.z, camUp.x, camUp.y, camUp.z);

        if (!renderer->BeginFrame()) continue;

        NkCamera3DData camData;
        camData.up = camUp;
        camData.fovY = 60.0f;
        camData.aspect = (float32)W / (float32)H;
        camData.nearPlane = 0.05f;
        camData.farPlane = 100.0f;

        NkCamera3D camera(camData);
        camera.SetPosition(camPos);
        camera.SetTarget(camTarget);

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

        // Sol, murs et fenêtre
        drawBox({0.0f, -0.05f, 0.0f}, {4.0f, 0.10f, 4.0f}, {0.35f, 0.35f, 0.38f});
        drawBox({0.0f, 1.25f, -2.00f}, {4.0f, 2.50f, 0.10f}, {0.70f, 0.72f, 0.75f});
        drawBox({2.0f, 1.25f, 0.0f}, {0.10f, 2.50f, 4.0f}, {0.68f, 0.70f, 0.72f});
        // Mur gauche avec ouverture fenêtre
        drawBox({-2.0f, 0.50f, 0.0f}, {0.10f, 1.00f, 4.0f}, {0.68f, 0.70f, 0.72f});
        drawBox({-2.0f, 2.15f, 0.0f}, {0.10f, 0.70f, 4.0f}, {0.68f, 0.70f, 0.72f});
        // Indicateur visuel à la fenêtre source sonore
        drawBox({-1.98f, 1.40f, -0.90f}, {0.05f, 0.60f, 0.60f}, {0.2f, 0.5f, 0.9f});

        renderer->Present();
        renderer->EndFrame();
        capture.ImageRendue();
    }

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();

    // Arrêt audio propre
    AudioEngine::Instance().Stop(voixPluie);
    AudioEngine::Instance().Shutdown();
    AudioLoader::Free(pluie);

    return capture.CodeDeSortie(0);
}
