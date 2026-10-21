#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "NKImage/Core/NkImage.h"

using namespace nkentseu;

static long GetFileSize(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        return (long)st.st_size;
    }
    return -1;
}

int main() {
    NkImage photo;
    if (!photo.Load("photo.jpg")) {
        // Essayer aussi avec le chemin complet si lancé depuis un autre dossier
        if (!photo.Load("chapitre-05/exo1-treize_formats_une_image/photo.jpg")) {
            std::cerr << "Erreur: Impossible de charger photo.jpg" << std::endl;
            return 1;
        }
    }

    std::cout << "Photo originale chargee avec succes: " << photo.Width() << " x " << photo.Height() << std::endl;

    // 5 formats de sauvegarde choisis parmi ceux du moteur :
    // PNG, JPEG, BMP, TGA, QOI
    const char *files[5] = {
        "image.png",
        "image.jpg",
        "image.bmp",
        "image.tga",
        "image.qoi"
    };

    const char *names[5] = {
        "png",
        "jpeg",
        "bmp",
        "tga",
        "qoi"
    };

    photo.SavePNG(files[0]);
    photo.SaveJPEG(files[1], 90);
    photo.SaveBMP(files[2]);
    photo.SaveTGA(files[3]);
    photo.SaveQOI(files[4]);

    std::ofstream outTxt("formats.txt");

    for (int i = 0; i < 5; ++i) {
        long sz = GetFileSize(files[i]);
        NkImage reloaded;
        bool ok = reloaded.Load(files[i]);

        if (ok && reloaded.IsValid()) {
            std::cout << names[i] << " : " << sz << " octets, " << reloaded.Width() << " x " << reloaded.Height() << std::endl;
            if (outTxt.is_open()) {
                outTxt << names[i] << " : " << sz << " octets, " << reloaded.Width() << " x " << reloaded.Height() << "\n";
            }
        } else {
            std::cout << names[i] << " : " << sz << " octets, illisible" << std::endl;
            if (outTxt.is_open()) {
                outTxt << names[i] << " : " << sz << " octets, illisible\n";
            }
        }
    }

    outTxt.close();
    return 0;
}
