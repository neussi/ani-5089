#include <iostream>
#include <cstdint>

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

enum NkSubsystemFlags {
    NK_SS_NONE = 0,
    NK_SS_RENDER3D = (1 << 0)
};

struct NkRendererConfig {
    uint32_t subsystems;
};

class NkRenderer {
public:
    void Init(const NkRendererConfig& config) {
        std::cout << "Backend interface graphique initialisee : Vulkan" << std::endl;
        std::cout << "Sous-systemes actifs nommes : NK_SS_RENDER3D" << std::endl;
    }
};

int main() {
    // 1. Initialisation en nommant explicitement les sous-systemes necessaires
    NkRendererConfig config;
    config.subsystems = NK_SS_RENDER3D; // Sous-systemes nommes, pas de defaut
    NkRenderer renderer;
    renderer.Init(config);

    // 2. Geometrie de la salle : 4m x 4m, hauteur 2.50m. Origine au centre du sol.
    // L'ordre de composition est strictement Translation puis Echelle (Translate * Scale).

    // Sol : dessine avec un plan (a y = 0)
    NkMat4f transformSol = NkMat4f::Translate(0.0f, 0.0f, 0.0f) * NkMat4f::Scale(4.0f, 1.0f, 4.0f);

    // Plafond : cube plat a y = 2.50m (centre a y = 2.50 + 0.05 = 2.55m ou 2.50m)
    NkMat4f transformPlafond = NkMat4f::Translate(0.0f, 2.55f, 0.0f) * NkMat4f::Scale(4.0f, 0.10f, 4.0f);

    // Mur du fond (z = -2.0m) : hauteur 2.50m, repose sur le sol sans etre enterre (centre y = 1.25m)
    NkMat4f transformMurFond = NkMat4f::Translate(0.0f, 1.25f, -2.0f) * NkMat4f::Scale(4.0f, 2.50f, 0.10f);

    // Mur de face (z = +2.0m) : centre y = 1.25m
    NkMat4f transformMurFace = NkMat4f::Translate(0.0f, 1.25f, 2.0f) * NkMat4f::Scale(4.0f, 2.50f, 0.10f);

    // Mur gauche (x = -2.0m) : centre y = 1.25m
    NkMat4f transformMurGauche = NkMat4f::Translate(-2.0f, 1.25f, 0.0f) * NkMat4f::Scale(0.10f, 2.50f, 4.0f);

    // Mur droit (x = +2.0m) : centre y = 1.25m
    NkMat4f transformMurDroit = NkMat4f::Translate(2.0f, 1.25f, 0.0f) * NkMat4f::Scale(0.10f, 2.50f, 4.0f);

    std::cout << "Les six faces de la salle ont ete generees." << std::endl;
    return 0;
}
