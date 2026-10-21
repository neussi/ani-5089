// =============================================================================
// Chapitre 05 - Exercice 7 : Un panneau au-dessus de la porte
// UE-ANI-IA-5049 — Coder pour la VR, XR et AR III
// =============================================================================

#include <iostream>
#include <cmath>
#include <fstream>
#include "NKPlatform/NkPlatformDetect.h"
#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKTime/NkTime.h"
#include "NKLogger/NkLog.h"

#include "NKFont/NkFont.h"
#include "NKFont/Embedded/NkFontEmbedded.h"
#include "NKImage/Core/NkImage.h"

#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"
#include "NKRenderer/Materials/NkMaterial.h"
#include "Commun/NkTutoCapture.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "Ch05Exo07Panneau";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
    (void)state;

    // 1. Construction de l'atlas avec la police embarquée Inter 32px
    NkFontAtlas atlas;
    NkFont *police = NkFontEmbedded::AddToAtlas(atlas, NkEmbeddedFontId::Inter, 32.f);
    if (!police) {
        std::cerr << "Erreur: AddToAtlas Inter a echoue" << std::endl;
        return 1;
    }
    if (!atlas.Build()) {
        std::cerr << "Erreur: atlas.Build a echoue" << std::endl;
        return 2;
    }

    nkft_uint8 *atlasPixels = nullptr;
    nkft_int32 texW = 0, texH = 0;
    atlas.GetTexDataAsAlpha8(&atlasPixels, &texW, &texH);

    const char *mot = "Sortie";
    float32 totalAdvance = 0.f;
    for (const char *p = mot; *p; ++p) {
        const NkFontGlyph *g = police->FindGlyph(*p);
        if (g) totalAdvance += g->advanceX;
    }

    int32 imgW = (int32)std::ceil(totalAdvance);
    int32 imgH = (int32)std::ceil(police->ascent - police->descent);
    int32 baseline = (int32)std::round(police->ascent);

    // Couleur fond vert sécurité (0x0A8C3CFF) et texte blanc pur
    math::NkColor bgCol(10, 140, 60, 255);
    math::NkColor textCol(255, 255, 255, 255);

    NkImage img = NkImage::Create((uint32)imgW, (uint32)imgH, 4, (uint32)0x0A8C3CFF);

    float32 cursorX = 0.f;
    for (const char *p = mot; *p; ++p) {
        const NkFontGlyph *g = police->FindGlyph(*p);
        if (!g) continue;

        int32 rx0 = (int32)std::floor(cursorX + g->x0);
        int32 rx1 = (int32)std::ceil(cursorX + g->x1);
        int32 ry0 = (int32)std::floor(baseline + g->y0);
        int32 ry1 = (int32)std::ceil(baseline + g->y1);

        for (int32 y = ry0; y < ry1; ++y) {
            if (y < 0 || y >= imgH) continue;
            float32 fy = (g->y1 > g->y0) ? float32(y - (baseline + g->y0)) / float32(g->y1 - g->y0) : 0.f;
            float32 v = g->v0 + fy * (g->v1 - g->v0);
            int32 py = int32(v * texH);
            if (py < 0) py = 0;
            if (py >= texH) py = texH - 1;

            for (int32 x = rx0; x < rx1; ++x) {
                if (x < 0 || x >= imgW) continue;
                float32 fx = (g->x1 > g->x0) ? float32(x - (cursorX + g->x0)) / float32(g->x1 - g->x0) : 0.f;
                float32 u = g->u0 + fx * (g->u1 - g->u0);
                int32 px = int32(u * texW);
                if (px < 0) px = 0;
                if (px >= texW) px = texW - 1;

                uint8 cov = atlasPixels[py * texW + px];
                float32 alpha = float32(cov) / 255.f;

                uint8 r = uint8(bgCol.r * (1.f - alpha) + textCol.r * alpha);
                uint8 g_c = uint8(bgCol.g * (1.f - alpha) + textCol.g * alpha);
                uint8 b = uint8(bgCol.b * (1.f - alpha) + textCol.b * alpha);

                img.SetPixel(x, y, math::NkColor(r, g_c, b, 255));
            }
        }
        cursorX += g->advanceX;
    }

    img.SavePNG("panneau.png");

    std::cout << "atlas : " << texW << " x " << texH << std::endl;
    std::cout << "image : " << imgW << " x " << imgH << std::endl;

    // 2. Fenêtre et rendu 3D
    NkWindowConfig winCfg;
    winCfg.title = "Exo 7 — Un panneau au-dessus de la porte";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;
    winCfg.resizable = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) return 3;

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) {
        window.Close();
        return 4;
    }

    NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 5;
    }

    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshHandle meshCube = renderer->GetMeshSystem()->GetCube();

    // Chargement de la texture du panneau
    NkTexHandle panTex = renderer->GetTextures()->Load("panneau.png");
    NkMaterial *matPanneau = NkMaterial::Create(renderer->GetMaterials(), NkMaterialType::NK_PBR_METALLIC);
    if (matPanneau) {
        matPanneau->SetAlbedoMap(panTex);
    }

    bool running = true;
    uint32 W = devInfo.width;
    uint32 H = devInfo.height;
    NkEventSystem &events = NkEvents();

    events.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) { running = false; });
    events.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() == NkKey::NK_ESCAPE) running = false;
    });

    NkClock clock;
    float32 totalTime = 0.0f;
    tuto::NkTutoCapture capture;

    const char *distEnv = getenv("DISTANCE_MODE");
    bool mode50cm = (distEnv && strcmp(distEnv, "50cm") == 0);

    // Dimensions panneau
    const float32 signH = 0.20f; // 20 cm de haut
    const float32 signW = signH * (float32(imgW) / float32(imgH)); // Proportions conservées
    const float32 signT = 0.02f;
    const float32 doorX = -0.70f;
    const float32 wallZ = -2.00f;
    const float32 doorH = 2.00f;
    const float32 signY = doorH + 0.12f; // Au-dessus de la porte (centre à y = 2.12m)
    const float32 signZ = wallZ + (signT * 0.5f) + 0.005f; // Décollé de 5 mm du mur

    while (running && window.IsOpen()) {
        events.PollEvents();
        if (!running) break;
        if (capture.Etape(renderer, device, W, H)) break;

        float32 dt = clock.Tick().delta;
        totalTime = capture.Temps(totalTime + dt);

        if (!renderer->BeginFrame()) continue;

        NkCamera3DData camData;
        camData.up = {0.0f, 1.0f, 0.0f};
        camData.fovY = 60.0f;
        camData.aspect = (float32)W / (float32)H;
        camData.nearPlane = 0.05f;
        camData.farPlane = 100.0f;

        NkCamera3D camera(camData);
        if (mode50cm) {
            // Cinquante centimètres : (-0.70 ; 2.20 ; -1.50)
            camera.SetPosition({-0.70f, 2.20f, -1.50f});
        } else {
            // Trois mètres : (-0.70 ; 1.70 ; 1.00)
            camera.SetPosition({-0.70f, 1.70f, 1.00f});
        }
        camera.SetTarget({doorX, signY, wallZ});

        NkSceneContext sctx;
        sctx.camera = camera;
        sctx.time = totalTime;

        NkLightDesc sun;
        sun.type = NkLightType::NK_DIRECTIONAL;
        sun.direction = {-0.3f, -0.7f, -0.5f};
        sun.color = {1.0f, 0.98f, 0.95f};
        sun.intensity = 2.6f;
        sun.castShadow = true;
        sctx.lights.PushBack(sun);

        r3d->BeginScene(sctx);

        auto drawBox = [&](NkVec3f center, NkVec3f size, NkVec3f tint, NkMaterial *mat = nullptr) {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            dc.transform = NkMat4f::Translate(center) * NkMat4f::Scale(size);
            dc.aabb = {
                {center.x - size.x * 0.5f, center.y - size.y * 0.5f, center.z - size.z * 0.5f},
                {center.x + size.x * 0.5f, center.y + size.y * 0.5f, center.z + size.z * 0.5f}
            };
            if (mat) {
                dc.material = mat->GetInstHandle();
            }
            dc.tint = tint;
            dc.roughness = 0.7f;
            dc.castShadow = true;
            r3d->Submit(dc);
        };

        // Sol
        drawBox({0.0f, -0.05f, 0.0f}, {4.0f, 0.10f, 4.0f}, {0.35f, 0.35f, 0.38f});
        // Mur fond (z = -2.0m)
        drawBox({0.0f, 1.25f, -2.00f}, {4.0f, 2.50f, 0.10f}, {0.70f, 0.72f, 0.75f});
        // Mur gauche
        drawBox({-2.0f, 1.25f, 0.0f}, {0.10f, 2.50f, 4.0f}, {0.68f, 0.70f, 0.72f});

        // Porte (x = -0.70m, y = 1.00m, hauteur 2.00m, largeur 0.90m)
        drawBox({doorX, 1.00f, wallZ + 0.015f}, {0.90f, doorH, 0.03f}, {0.45f, 0.28f, 0.15f});

        // Panneau "Sortie" au-dessus de la porte avec texture
        drawBox({doorX, signY, signZ}, {signW, signH, signT}, {1.0f, 1.0f, 1.0f}, matPanneau);

        renderer->Present();
        renderer->EndFrame();
        capture.ImageRendue();
    }

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return capture.CodeDeSortie(0);
}
