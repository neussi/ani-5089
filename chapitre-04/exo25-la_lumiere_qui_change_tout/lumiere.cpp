#include <iostream>
#include <cstdint>

struct NkVec3f {
    float x, y, z;
    NkVec3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) : x(_x), y(_y), z(_z) {}
};

enum class NkLightType {
    NK_DIRECTIONAL = 0,
    NK_POINT = 1,
    NK_SPOT = 2
};

struct NkLight {
    NkLightType type;
    NkVec3f direction;
    NkVec3f color;
    float intensity;
    bool castShadow;
};

enum NkSubsystemFlags {
    NK_SS_NONE = 0,
    NK_SS_RENDER3D = (1 << 0),
    NK_SS_SHADOW   = (1 << 1)
};

struct NkRendererConfig {
    uint32_t subsystems;
};

class NkRenderer {
public:
    void Init(const NkRendererConfig& config) {
        std::cout << "Renderer allume avec NK_SS_RENDER3D et NK_SS_SHADOW" << std::endl;
    }
    void SetSun(const NkLight& sun) {
        std::cout << "Soleil configure: direction(" << sun.direction.x << "," << sun.direction.y << "," << sun.direction.z
                  << "), intensite=" << sun.intensity << ", castShadow=" << (sun.castShadow ? "true" : "false") << std::endl;
    }
};

int main() {
    NkRenderer renderer;
    NkRendererConfig config = {};
    // Le sous-systeme des ombres est allume en plus du rendu 3D
    config.subsystems = NK_SS_RENDER3D | NK_SS_SHADOW;
    renderer.Init(config);

    // 1. Reglage de depart (depart.png)
    NkLight soleilDepart;
    soleilDepart.type = NkLightType::NK_DIRECTIONAL;
    soleilDepart.direction = NkVec3f(-0.4f, -1.0f, -0.3f);
    soleilDepart.color = NkVec3f(1.0f, 0.95f, 0.85f);
    soleilDepart.intensity = 3.0f;
    soleilDepart.castShadow = true;
    renderer.SetSun(soleilDepart);

    // 2. Variation de direction seule (direction.png) : lumiere rasante
    NkLight soleilDirection = soleilDepart;
    soleilDirection.direction = NkVec3f(0.85f, -0.20f, -0.25f);

    // 3. Variation d'intensite seule (intensite.png) : eclairage sous-expose
    NkLight soleilIntensite = soleilDepart;
    soleilIntensite.intensity = 0.6f;

    // 4. Desactivation des ombres portees (ombres.png)
    NkLight soleilSansOmbres = soleilDepart;
    soleilSansOmbres.castShadow = false;

    return 0;
}
