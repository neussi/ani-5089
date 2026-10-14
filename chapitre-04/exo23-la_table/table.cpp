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
    // Position du centre de la table au sol
    const float tableX = 0.60f;
    const float tableZ = 0.70f;

    // Dimensions du plateau de table (Top)
    const float plateauLongueurX = 1.20f; // 1.20 m de long selon x
    const float plateauProfondeurZ = 0.80f; // 0.80 m de profondeur selon z
    const float plateauEpaisseurY = 0.05f; // 5 cm d'epaisseur
    const float hauteurFaceSuperieure = 0.75f; // Face superieure a 0.75 m du sol

    // Calcul du centre y du plateau :
    // y_centre = hauteurFaceSuperieure - (plateauEpaisseurY / 2.0f) = 0.75 - 0.025 = 0.725 m
    const float plateauCentreY = hauteurFaceSuperieure - (plateauEpaisseurY * 0.5f); // 0.725f

    NkMat4f plateauTransform = NkMat4f::Translate(tableX, plateauCentreY, tableZ) *
                               NkMat4f::Scale(plateauLongueurX, plateauEpaisseurY, plateauProfondeurZ);

    // Dimensions et calculs des 4 pieds (Legs)
    // Les pieds vont du sol (y = 0) jusqu'au-dessous du plateau (y = 0.75 - 0.05 = 0.70 m)
    const float sectionPiedX = 0.06f; // 6 cm de section
    const float sectionPiedZ = 0.06f; // 6 cm de section
    const float hauteurPiedsY = hauteurFaceSuperieure - plateauEpaisseurY; // 0.75 - 0.05 = 0.70 m
    const float piedsCentreY = hauteurPiedsY * 0.5f; // 0.70 / 2 = 0.35 m

    // Coordonnees x et z des 4 pieds (places aux 4 coins sous le plateau sans depasser)
    // Le plateau s'etend de [0.60 - 0.60, 0.60 + 0.60] = [0.00, 1.20] en x
    // et de [0.70 - 0.40, 0.70 + 0.40] = [0.30, 1.10] en z
    const float demiLongueur = plateauLongueurX * 0.5f;   // 0.60
    const float demiProfondeur = plateauProfondeurZ * 0.5f; // 0.40
    const float demiSection = sectionPiedX * 0.5f;        // 0.03

    const float piedMinX = (tableX - demiLongueur) + demiSection; // 0.03f
    const float piedMaxX = (tableX + demiLongueur) - demiSection; // 1.17f
    const float piedMinZ = (tableZ - demiProfondeur) + demiSection; // 0.33f
    const float piedMaxZ = (tableZ + demiProfondeur) - demiSection; // 1.07f

    // Matrices des 4 pieds
    NkMat4f pied1 = NkMat4f::Translate(piedMinX, piedsCentreY, piedMinZ) * NkMat4f::Scale(sectionPiedX, hauteurPiedsY, sectionPiedZ);
    NkMat4f pied2 = NkMat4f::Translate(piedMaxX, piedsCentreY, piedMinZ) * NkMat4f::Scale(sectionPiedX, hauteurPiedsY, sectionPiedZ);
    NkMat4f pied3 = NkMat4f::Translate(piedMinX, piedsCentreY, piedMaxZ) * NkMat4f::Scale(sectionPiedX, hauteurPiedsY, sectionPiedZ);
    NkMat4f pied4 = NkMat4f::Translate(piedMaxX, piedsCentreY, piedMaxZ) * NkMat4f::Scale(sectionPiedX, hauteurPiedsY, sectionPiedZ);

    std::cout << "Table placee a x = " << tableX << ", z = " << tableZ << std::endl;
    std::cout << "Plateau dimensions: " << plateauLongueurX << " x " << plateauEpaisseurY << " x " << plateauProfondeurZ << std::endl;
    std::cout << "Centre Y du plateau: " << plateauCentreY << " m" << std::endl;
    std::cout << "Hauteur des pieds: " << hauteurPiedsY << " m, Centre Y des pieds: " << piedsCentreY << " m" << std::endl;

    return 0;
}
