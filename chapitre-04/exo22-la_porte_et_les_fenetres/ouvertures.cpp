#include <iostream>

struct NkVec3f {
    float x, y, z;
    NkVec3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) : x(_x), y(_y), z(_z) {}
};

struct NkMat4f {
    float m[16];
    static NkMat4f Identity() {
        NkMat4f r = {};
        r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f;
        return r;
    }
    static NkMat4f Translate(float tx, float ty, float tz) {
        NkMat4f r = Identity();
        r.m[12] = tx; r.m[13] = ty; r.m[14] = tz;
        return r;
    }
    static NkMat4f Scale(float sx, float sy, float sz) {
        NkMat4f r = Identity();
        r.m[0] = sx; r.m[5] = sy; r.m[10] = sz;
        return r;
    }
    NkMat4f operator*(const NkMat4f& o) const {
        NkMat4f r = {};
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                for (int k = 0; k < 4; ++k) {
                    r.m[i * 4 + j] += m[i * 4 + k] * o.m[k * 4 + j];
                }
            }
        }
        return r;
    }
};

int main() {
    // Dimensions de la salle : interieur 4m x 4m, hauteur 2.50m, murs epaisseur 0.10m.
    // Origine au centre du sol (y = 0).
    // Mur du fond a z = -2.0m. Mur gauche a x = -2.0m.
    
    // Decalage pour eliminer le z-fighting (deplacement vers l'interieur de la piece)
    const float offset = 0.005f; // 5 millimetres (0.005 m)

    // --- Porte ---
    // Largeur = 0.90m, Hauteur = 2.00m, Epaisseur = 0.02m (2 cm)
    // Posee au sol (centre y = 2.00 / 2 = 1.00m), dans le mur du fond (z = -2.0m)
    const float porteLargeur = 0.90f;
    const float porteHauteur = 2.00f;
    const float porteEpaisseur = 0.02f;
    const float porteX = -0.70f;
    const float porteY = 1.00f; // centre a mi-hauteur pour poser au sol
    const float porteZ = -2.00f + offset; // avancee vers l'interieur (+z)

    NkMat4f porteTransform = NkMat4f::Translate(porteX, porteY, porteZ) * 
                             NkMat4f::Scale(porteLargeur, porteHauteur, porteEpaisseur);

    // --- Deux Fenetres ---
    // Largeur = 1.20m (le long de z), Hauteur = 1.00m (le long de y), Epaisseur = 0.02m (le long de x)
    // Dans le mur gauche (x = -2.0m), centre a y = 1.40m au-dessus du sol
    const float fenetreLargeurZ = 1.20f; // largeur comptee selon z sur un mur lateral
    const float fenetreHauteurY = 1.00f; // hauteur de la fenetre
    const float fenetreEpaisseurX = 0.02f;
    const float fenetreCentreY = 1.40f; // hauteur du centre des fenetres
    const float fenetreX = -2.00f + offset; // avancee vers l'interieur (+x)

    // Fenetre 1 : centree a z = -0.90m
    const float fenetre1Z = -0.90f;
    NkMat4f fenetre1Transform = NkMat4f::Translate(fenetreX, fenetreCentreY, fenetre1Z) *
                               NkMat4f::Scale(fenetreEpaisseurX, fenetreHauteurY, fenetreLargeurZ);

    // Fenetre 2 : centree a z = +0.90m
    const float fenetre2Z = +0.90f;
    NkMat4f fenetre2Transform = NkMat4f::Translate(fenetreX, fenetreCentreY, fenetre2Z) *
                               NkMat4f::Scale(fenetreEpaisseurX, fenetreHauteurY, fenetreLargeurZ);

    std::cout << "Porte placee en x = " << porteX << ", y = " << porteY << ", z = " << porteZ << std::endl;
    std::cout << "Fenetres largeur = " << fenetreLargeurZ << ", hauteur centre = " << fenetreCentreY << std::endl;

    return 0;
}
