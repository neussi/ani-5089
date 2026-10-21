#include <iostream>
#include <fstream>
#include <vector>
#include "NKImage/Core/NkImage.h"
#include "NKTime/NkChrono.h"

using namespace nkentseu;

int main() {
    NkImage rawPhoto;
    if (!rawPhoto.Load("photo.jpg")) {
        if (!rawPhoto.Load("chapitre-05/exo5-quatre_filtres_une_mesure/photo.jpg")) {
            std::cerr << "Erreur: photo.jpg introuvable" << std::endl;
            return 1;
        }
    }

    // Préparation de la source en 2048 x 2048
    NkImage source = rawPhoto.Resize(2048, 2048, NkResizeFilter::NK_BILINEAR);
    if (!source.IsValid() || source.Width() != 2048 || source.Height() != 2048) {
        std::cerr << "Erreur: redimensionnement source 2048x2048 echoue" << std::endl;
        return 2;
    }

    struct FiltreTest {
        const char *nom;
        NkResizeFilter filtre;
        const char *fichier;
        NkImage resultat;
        uint64 tempsMoyenUs;
        int differences;
    };

    FiltreTest tests[4] = {
        {"NK_NEAREST",  NkResizeFilter::NK_NEAREST,  "nearest.png",  {}, 0, 0},
        {"NK_BILINEAR", NkResizeFilter::NK_BILINEAR, "bilinear.png", {}, 0, 0},
        {"NK_BICUBIC",  NkResizeFilter::NK_BICUBIC,  "bicubic.png",  {}, 0, 0},
        {"NK_LANCZOS3", NkResizeFilter::NK_LANCZOS3, "lanczos3.png", {}, 0, 0}
    };

    const int NB_REPETITIONS = 20;

    for (int f = 0; f < 4; ++f) {
        // Un premier redimensionnement de chauffe (warmup) et conservation de l'image
        tests[f].resultat = source.Resize(256, 256, tests[f].filtre);
        tests[f].resultat.SavePNG(tests[f].fichier);

        uint64 totalUs = 0;
        for (int rep = 0; rep < NB_REPETITIONS; ++rep) {
            NkChrono chrono;
            NkImage img = source.Resize(256, 256, tests[f].filtre);
            totalUs += (uint64)chrono.Elapsed().ToMicroseconds();
        }
        tests[f].tempsMoyenUs = totalUs / NB_REPETITIONS;
    }

    // Référence : bilinear
    const NkImage &ref = tests[1].resultat;

    for (int f = 0; f < 4; ++f) {
        int diffCount = 0;
        const NkImage &target = tests[f].resultat;
        for (int y = 0; y < 256; ++y) {
            for (int x = 0; x < 256; ++x) {
                math::NkColor c1 = ref.GetPixel(x, y);
                math::NkColor c2 = target.GetPixel(x, y);
                if (c1.r != c2.r || c1.g != c2.g || c1.b != c2.b || c1.a != c2.a) {
                    diffCount++;
                }
            }
        }
        tests[f].differences = diffCount;
    }

    std::ofstream outTxt("mesures.txt");
    for (int f = 0; f < 4; ++f) {
        std::cout << tests[f].nom << " : " << tests[f].tempsMoyenUs << " us, differences : " << tests[f].differences << std::endl;
        if (outTxt.is_open()) {
            outTxt << tests[f].nom << " : " << tests[f].tempsMoyenUs << " us, differences : " << tests[f].differences << "\n";
        }
    }
    outTxt.close();

    return 0;
}
