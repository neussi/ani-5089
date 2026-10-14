#include <iostream>
#include <chrono>
#include <string>

enum class NkGraphicsApi {
    NK_GFX_API_SOFTWARE = 0,
    NK_GFX_API_OPENGL   = 1,
    NK_GFX_API_DX11     = 2,
    NK_GFX_API_DX12     = 3,
    NK_GFX_API_VULKAN   = 4
};

const char* NkGraphicsApiName(NkGraphicsApi api) {
    switch (api) {
        case NkGraphicsApi::NK_GFX_API_SOFTWARE: return "Software";
        case NkGraphicsApi::NK_GFX_API_OPENGL:   return "OpenGL";
        case NkGraphicsApi::NK_GFX_API_DX11:     return "DirectX 11";
        case NkGraphicsApi::NK_GFX_API_DX12:     return "DirectX 12";
        case NkGraphicsApi::NK_GFX_API_VULKAN:   return "Vulkan";
        default: return "Inconnu";
    }
}

struct NkDeviceInitInfo {
    int width = 1280;
    int height = 720;
    bool vsync = false;
};

class NkDevice {
public:
    bool IsValid() const { return true; }
};

class NkDeviceFactory {
public:
    static NkDevice* CreateForApi(NkGraphicsApi api, const NkDeviceInitInfo& info) {
        return new NkDevice();
    }
};

struct NkRendererConfig {
    NkGraphicsApi api;
    uint32_t subsystems;
};

class NkRenderer {
public:
    bool Init(const NkRendererConfig& config) {
        return true;
    }
    void RenderFrame() {}
};

void ExecuterMesure(NkGraphicsApi api) {
    std::cout << "Demarrage avec l'interface : " << NkGraphicsApiName(api) << std::endl;

    NkDeviceInitInfo initInfo;
    initInfo.vsync = false;

    // Creation forcee avec CreateForApi
    NkDevice* device = NkDeviceFactory::CreateForApi(api, initInfo);
    if (!device || !device->IsValid()) {
        std::cerr << "Echec d'initialisation du device pour " << NkGraphicsApiName(api) << std::endl;
        return;
    }

    NkRendererConfig config;
    config.api = api; // On transmet la meme interface au renderer
    config.subsystems = 1;

    NkRenderer renderer;
    if (!renderer.Init(config)) {
        std::cerr << "Echec d'initialisation du renderer pour " << NkGraphicsApiName(api) << std::endl;
        return;
    }

    // Phase d'echauffement (chargement) ignoree
    for (int i = 0; i < 20; ++i) {
        renderer.RenderFrame();
    }

    // Mesure du temps moyen sur 400 images
    const int nbFrames = 400;
    auto debut = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < nbFrames; ++i) {
        renderer.RenderFrame();
    }
    auto fin = std::chrono::high_resolution_clock::now();

    double dureeTotaleMs = std::chrono::duration<double, std::milli>(fin - debut).count();
    double tempsMoyenMs = dureeTotaleMs / nbFrames;

    std::cout << "Interface: " << NkGraphicsApiName(api) << " | Temps moyen par image : " << tempsMoyenMs << " ms" << std::endl;
}

int main() {
    // 1. Premiere interface : Vulkan (ou OpenGL materiel)
    ExecuterMesure(NkGraphicsApi::NK_GFX_API_VULKAN);

    // 2. Seconde interface : Rendu logiciel (Software)
    ExecuterMesure(NkGraphicsApi::NK_GFX_API_SOFTWARE);

    return 0;
}
