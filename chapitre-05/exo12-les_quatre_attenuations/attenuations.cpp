// =============================================================================
// Chapitre 05 - Exercice 12 : Les quatre atténuations
// attenuations.cpp : Mesure du gain des 4 modèles d'atténuation sonore du moteur
// UE-ANI-IA-5049 — Coder pour la VR, XR et AR III
// =============================================================================

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>

#include "NKAudio/NkAudio.h"
#include "NKWindow/NKMain.h"

using namespace nkentseu;
using namespace nkentseu::audio;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "Ch05Exo12Attenuations";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

struct ModelInfo {
    AttenuationModel model;
    const char *nom;
};

int nkmain(const NkEntryState &state) {
    (void)state;

    // 1. Initialisation avec backend NULL_OUTPUT (aucun périphérique requis)
    AudioEngineConfig cfg;
    cfg.sampleRate = 48000;
    cfg.channels = 2;
    cfg.backend = AudioBackendType::NULL_OUTPUT;
    AudioEngine::Instance().Initialize(cfg);

    // 2. Position et orientation de l'auditeur à l'origine (0, 0, 0) regardant vers -Z
    AudioEngine::Instance().SetListenerPosition(0.0f, 0.0f, 0.0f);
    AudioEngine::Instance().SetListenerOrientation(0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

    // 3. Génération d'une sinusoïde douce (440 Hz, durée 2 s, amplitude 0.1)
    AudioSample tone = AudioGenerator::GenerateTone(440.0f, 2.0f, WaveformType::SINE, 48000, 0.1f);

    const float32 distances[] = {0.5f, 1.0f, 2.0f, 4.0f, 8.0f};
    const int numDistances = 5;

    const int rolloffs[] = {1, 2};

    ModelInfo models[] = {
        {AttenuationModel::NONE, "NONE"},
        {AttenuationModel::INVERSE, "INVERSE"},
        {AttenuationModel::LINEAR, "LINEAR"},
        {AttenuationModel::EXPONENTIAL, "EXPONENTIAL"}
    };

    const int32 totalFrames = 48000;
    const int32 blockSize = 256;
    float32 buffer[blockSize * 2]; // Stéréo L, R interleaved

    // Fonction de mesure RMS sur les deux oreilles ensemble
    auto measureRMS = [&](AttenuationModel model, float32 rolloff, float32 d) -> double {
        VoiceParams p;
        p.looping = true;
        p.volume = 1.0f;
        p.source3d.positional = true;
        p.source3d.position[0] = 0.0f;
        p.source3d.position[1] = 0.0f;
        p.source3d.position[2] = -d;
        p.source3d.minDistance = 1.0f;
        p.source3d.maxDistance = 100.0f;
        p.source3d.rolloffFactor = rolloff;
        p.source3d.attenuationModel = model;

        AudioHandle voix = AudioEngine::Instance().Play(tone, p);

        double sumSqBoth = 0.0;
        int32 framesRendered = 0;

        while (framesRendered < totalFrames) {
            int32 framesToRender = std::min(blockSize, totalFrames - framesRendered);
            AudioEngine::Instance().RenderToBuffer(buffer, framesToRender, 2);

            for (int32 i = 0; i < framesToRender; ++i) {
                float32 sL = buffer[i * 2 + 0];
                float32 sR = buffer[i * 2 + 1];
                sumSqBoth += (double)sL * (double)sL + (double)sR * (double)sR;
            }
            framesRendered += framesToRender;
        }

        AudioEngine::Instance().Stop(voix);

        // RMS combiné sur les deux oreilles (2 * totalFrames échantillons)
        return std::sqrt(sumSqBoth / (2.0 * totalFrames));
    };

    std::ofstream fichier("mesures.txt");

    std::cout << "--- Mesures des 4 modeles d'attenuation ---" << std::endl;

    for (int r : rolloffs) {
        float32 rolloffVal = (float32)r;

        // Mesure préliminaire de référence NONE pour chaque distance
        double baseNoneRMS[5];
        for (int i = 0; i < numDistances; ++i) {
            baseNoneRMS[i] = measureRMS(AttenuationModel::NONE, rolloffVal, distances[i]);
            if (baseNoneRMS[i] <= 1e-9) baseNoneRMS[i] = 1.0;
        }

        for (const auto &m : models) {
            for (int i = 0; i < numDistances; ++i) {
                float32 d = distances[i];
                double rms = measureRMS(m.model, rolloffVal, d);
                double gain = rms / baseNoneRMS[i];

                std::string distStr;
                if (d == 0.5f) {
                    distStr = "0.5 m";
                } else {
                    distStr = std::to_string((int)d) + " m";
                }

                // Format demandé : INVERSE, rolloff 1, 2 m : g
                std::cout << m.nom << ", rolloff " << r << ", " << distStr << " : "
                          << std::fixed << std::setprecision(3) << gain << std::endl;

                if (fichier.is_open()) {
                    fichier << m.nom << ", rolloff " << r << ", " << distStr << " : "
                            << std::fixed << std::setprecision(3) << gain << "\n";
                }
            }
        }
    }

    if (fichier.is_open()) {
        fichier.close();
    }

    // 4. Fermeture propre du moteur
    AudioEngine::Instance().Shutdown();
    AudioLoader::Free(tone);

    return 0;
}
