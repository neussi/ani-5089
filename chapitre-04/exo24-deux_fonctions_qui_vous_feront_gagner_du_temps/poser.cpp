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

// Fonction 1 : Pose un objet au sol (y = 0)
// Compose la translation puis l'echelle (Translate * Scale)
NkMat4f PoserAuSol(NkVec3f taille, float x, float z) {
    float centreY = taille.y * 0.5f;
    return NkMat4f::Translate(x, centreY, z) * NkMat4f::Scale(taille.x, taille.y, taille.z);
}

// Fonction 2 : Pose un objet sur la face superieure de la table (y = 0.75 m)
// Compose la translation puis l'echelle (Translate * Scale)
NkMat4f PoserSurTable(NkVec3f taille, float x, float z) {
    float centreY = 0.75f + (taille.y * 0.5f);
    return NkMat4f::Translate(x, centreY, z) * NkMat4f::Scale(taille.x, taille.y, taille.z);
}

int main() {
    // 1. Murs de la salle (4m x 4m x 2.50m) poses au sol avec PoserAuSol
    NkMat4f murFond   = PoserAuSol(NkVec3f(4.0f, 2.5f, 0.1f), 0.0f, -2.0f);
    NkMat4f murFace   = PoserAuSol(NkVec3f(4.0f, 2.5f, 0.1f), 0.0f,  2.0f);
    NkMat4f murGauche = PoserAuSol(NkVec3f(0.1f, 2.5f, 4.0f), -2.0f, 0.0f);
    NkMat4f murDroite = PoserAuSol(NkVec3f(0.1f, 2.5f, 4.0f),  2.0f, 0.0f);

    // 2. Porte (0.90m x 2.00m x 0.02m) posee au sol
    NkMat4f porte = PoserAuSol(NkVec3f(0.90f, 2.00f, 0.02f), -0.70f, -1.995f);

    // 3. Pieds de table poses au sol (section 0.06 x 0.06, hauteur 0.70m)
    NkMat4f pied1 = PoserAuSol(NkVec3f(0.06f, 0.70f, 0.06f), 0.03f, 0.33f);
    NkMat4f pied2 = PoserAuSol(NkVec3f(0.06f, 0.70f, 0.06f), 1.17f, 0.33f);
    NkMat4f pied3 = PoserAuSol(NkVec3f(0.06f, 0.70f, 0.06f), 0.03f, 1.07f);
    NkMat4f pied4 = PoserAuSol(NkVec3f(0.06f, 0.70f, 0.06f), 1.17f, 1.07f);

    // 4. Plateau de table (epaisseur 0.05m, pose a y = 0.70m pour que le haut soit a 0.75m)
    NkMat4f plateau = NkMat4f::Translate(0.60f, 0.725f, 0.70f) * NkMat4f::Scale(1.20f, 0.05f, 0.80f);

    // 5. Objet pose sur la table avec PoserSurTable (une boite de 15cm x 10cm x 15cm)
    NkVec3f tailleObjet(0.15f, 0.10f, 0.15f);
    NkMat4f objetSurTable = PoserSurTable(tailleObjet, 0.60f, 0.70f);

    std::cout << "Tous les elements au sol et sur la table ont ete positionnes." << std::endl;
    return 0;
}
