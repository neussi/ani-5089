// =============================================================================
// Chapitre 05 - Exercice 11 : Le son qui a une place
// mesure.cpp : Mesure précise de la spatialisation 3D (RMS gauche / droite)
// UE-ANI-IA-5049 — Coder pour la VR, XR et AR III
// =============================================================================

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <iomanip>

#include "NKAudio/NkAudio.h"
#include "NKWindow/NKMain.h"

using namespace nkentseu;
using namespace nkentseu::audio;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "Ch05Exo11Mesure";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

struct PositionTest {
    const char *nom;
    float32 x, y, z;
};

int nkmain(const NkEntryState &state) {
    (void)state;

    // 1. Initialisation avec backend NULL_OUTPUT (aucun périphérique requis)
    AudioEngineConfig cfg;
    cfg.sampleRate = 48000;
    cfg.channels = 2;
    cfg.backend = AudioBackendType::NULL_OUTPUT;
    AudioEngine::Instance().Initialize(cfg);

    // 2. Position et orientation de l'auditeur à l'origine regardant vers -Z
    AudioEngine::Instance().SetListenerPosition(0.0f, 0.0f, 0.0f);
    AudioEngine::Instance().SetListenerOrientation(0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

    // 3. Génération du bruit rose (pluie)
    AudioSample pluie = AudioGenerator::GenerateNoise(4.0f, WaveformType::NOISE_PINK);

    PositionTest positions[] = {
        {"devant", 0.0f, 0.0f, -2.0f},
        {"gauche", -2.0f, 0.0f, 0.0f},
        {"droite", 2.0f, 0.0f, 0.0f}
    };

    std::ofstream fichier("mesures.txt");

    const int32 totalFrames = 48000;
    const int32 blockSize = 256;
    float32 buffer[blockSize * 2]; // Stéréo L, R interleaved

    std::cout << "--- Mesures de spatialisation audio ---" << std::endl;

    for (const auto &pos : positions) {
        // Test avec positional = true (1), puis positional = false (0)
        for (int pFlag = 1; pFlag >= 0; --pFlag) {
            VoiceParams p;
            p.looping = true;
            p.volume = 1.0f;
            p.source3d.positional = (pFlag == 1);
            p.source3d.position[0] = pos.x;
            p.source3d.position[1] = pos.y;
            p.source3d.position[2] = pos.z;
            p.source3d.minDistance = 1.0f;
            p.source3d.maxDistance = 20.0f;
            p.source3d.rolloffFactor = 1.0f;
            p.source3d.attenuationModel = AttenuationModel::INVERSE;

            AudioHandle voix = AudioEngine::Instance().Play(pluie, p);

            double sumSqL = 0.0;
            double sumSqR = 0.0;
            int32 framesRendered = 0;

            while (framesRendered < totalFrames) {
                int32 framesToRender = std::min(blockSize, totalFrames - framesRendered);
                AudioEngine::Instance().RenderToBuffer(buffer, framesToRender, 2);

                for (int32 i = 0; i < framesToRender; ++i) {
                    float32 sL = buffer[i * 2 + 0];
                    float32 sR = buffer[i * 2 + 1];
                    sumSqL += (double)sL * (double)sL;
                    sumSqR += (double)sR * (double)sR;
                }
                framesRendered += framesToRender;
            }

            // Arrêt de la voix avant de passer au test suivant
            AudioEngine::Instance().Stop(voix);

            double rmsL = std::sqrt(sumSqL / totalFrames);
            double rmsR = std::sqrt(sumSqR / totalFrames);

            // Format demandé : devant 1 : G x D y
            std::cout << pos.nom << " " << pFlag << " : G " << std::fixed << std::setprecision(4) << rmsL
                      << " D " << std::fixed << std::setprecision(4) << rmsR << std::endl;

            if (fichier.is_open()) {
                fichier << pos.nom << " " << pFlag << " : G " << std::fixed << std::setprecision(4) << rmsL
                        << " D " << std::fixed << std::setprecision(4) << rmsR << "\n";
            }
        }
    }

    if (fichier.is_open()) {
        fichier.close();
    }

    // 4. Fermeture propre du moteur
    AudioEngine::Instance().Shutdown();
    AudioLoader::Free(pluie);

    return 0;
}
