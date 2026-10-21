// =============================================================================
// Chapitre 05 - Exercice 8 : Le SDF mesuré
// UE-ANI-IA-5049 — Coder pour la VR, XR et AR III
// =============================================================================

#include <iostream>
#include <cmath>
#include <algorithm>
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
    d.appName = "Ch05Exo08Sdf";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

inline float smoothstep(float a, float b, float x) {
    float t = std::clamp((x - a) / (b - a), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

inline float SampleAtlasBilinear(const nkft_uint8 *atlas, int32 w, int32 h, float u, float v) {
    float px = u * float(w) - 0.5f;
    float py = v * float(h) - 0.5f;
    int32 x0 = int32(std::floor(px));
    int32 y0 = int32(std::floor(py));
    int32 x1 = x0 + 1;
    int32 y1 = y0 + 1;
    float fx = px - float(x0);
    float fy = py - float(y0);

    auto getVal = [&](int32 x, int32 y) -> float {
        if (x < 0) x = 0;
        if (x >= w) x = w - 1;
        if (y < 0) y = 0;
        if (y >= h) y = h - 1;
        return float(atlas[y * w + x]);
    };

    float v00 = getVal(x0, y0);
    float v10 = getVal(x1, y0);
    float v01 = getVal(x0, y1);
    float v11 = getVal(x1, y1);

    float top = v00 * (1.f - fx) + v10 * fx;
    float bot = v01 * (1.f - fx) + v11 * fx;
    return top * (1.f - fy) + bot * fy;
}

int nkmain(const NkEntryState &state) {
    (void)state;

    // 1. Construction des 3 atlas
    // Atlas 1 : Ordinaire 32 px
    NkFontAtlas atlasOrd32;
    NkFont *fontOrd32 = NkFontEmbedded::AddToAtlas(atlasOrd32, NkEmbeddedFontId::Inter, 32.f);
    atlasOrd32.Build();

    // Atlas 2 : SDF 32 px (sdfMode = true, sdfSpread = 6)
    NkFontAtlas atlasSdf32;
    atlasSdf32.sdfMode = true;
    atlasSdf32.sdfSpread = 6;
    NkFont *fontSdf32 = NkFontEmbedded::AddToAtlas(atlasSdf32, NkEmbeddedFontId::Inter, 32.f);
    atlasSdf32.Build();

    // Atlas 3 : Ordinaire 128 px
    NkFontAtlas atlasOrd128;
    NkFont *fontOrd128 = NkFontEmbedded::AddToAtlas(atlasOrd128, NkEmbeddedFontId::Inter, 128.f);
    atlasOrd128.Build();

    // Écriture du fichier atlas.txt avec les tailles exactes
    std::ofstream outAtlas("atlas.txt");
    if (outAtlas.is_open()) {
        outAtlas << "ordinaire 32 : " << atlasOrd32.texWidth << " x " << atlasOrd32.texHeight << "\n";
        outAtlas << "sdf 32 : " << atlasSdf32.texWidth << " x " << atlasSdf32.texHeight << "\n";
        outAtlas << "ordinaire 128 : " << atlasOrd128.texWidth << " x " << atlasOrd128.texHeight << "\n";
        outAtlas.close();
    }
    std::cout << "ordinaire 32 : " << atlasOrd32.texWidth << " x " << atlasOrd32.texHeight << std::endl;
    std::cout << "sdf 32 : " << atlasSdf32.texWidth << " x " << atlasSdf32.texHeight << std::endl;
    std::cout << "ordinaire 128 : " << atlasOrd128.texWidth << " x " << atlasOrd128.texHeight << std::endl;

    // 2. Génération des images agrandies 8x (même taille pour les deux)
    nkft_uint8 *pixOrd32 = nullptr;
    nkft_int32 wOrd32 = 0, hOrd32 = 0;
    atlasOrd32.GetTexDataAsAlpha8(&pixOrd32, &wOrd32, &hOrd32);

    nkft_uint8 *pixSdf32 = nullptr;
    nkft_int32 wSdf32 = 0, hSdf32 = 0;
    atlasSdf32.GetTexDataAsAlpha8(&pixSdf32, &wSdf32, &hSdf32);

    const char *mot = "Sortie";
    float32 totalAdvance = 0.f;
    for (const char *p = mot; *p; ++p) {
        const NkFontGlyph *g = fontOrd32->FindGlyph(*p);
        if (g) totalAdvance += g->advanceX;
    }

    const float32 scale8x = 8.0f;
    int32 imgW = int32(std::ceil(totalAdvance * scale8x));
    int32 imgH = int32(std::ceil((fontOrd32->ascent - fontOrd32->descent) * scale8x));
    int32 baseline = int32(std::round(fontOrd32->ascent * scale8x));

    math::NkColor bgCol(10, 140, 60, 255);
    math::NkColor textCol(255, 255, 255, 255);

    NkImage imgOrd = NkImage::Create((uint32)imgW, (uint32)imgH, 4, (uint32)0x0A8C3CFF);
    NkImage imgSdf = NkImage::Create((uint32)imgW, (uint32)imgH, 4, (uint32)0x0A8C3CFF);

    float32 curX = 0.f;
    for (const char *p = mot; *p; ++p) {
        const NkFontGlyph *gOrd = fontOrd32->FindGlyph(*p);
        const NkFontGlyph *gSdf = fontSdf32->FindGlyph(*p);
        if (!gOrd || !gSdf) continue;

        int32 rx0 = (int32)std::floor(curX + gOrd->x0 * scale8x);
        int32 rx1 = (int32)std::ceil(curX + gOrd->x1 * scale8x);
        int32 ry0 = (int32)std::floor(baseline + gOrd->y0 * scale8x);
        int32 ry1 = (int32)std::ceil(baseline + gOrd->y1 * scale8x);

        for (int32 y = ry0; y < ry1; ++y) {
            if (y < 0 || y >= imgH) continue;
            float32 fy = (ry1 > ry0) ? float32(y - (baseline + gOrd->y0 * scale8x)) / float32(gOrd->y1 * scale8x - gOrd->y0 * scale8x) : 0.f;
            float32 vOrd = gOrd->v0 + fy * (gOrd->v1 - gOrd->v0);
            float32 vSdf = gSdf->v0 + fy * (gSdf->v1 - gSdf->v0);

            for (int32 x = rx0; x < rx1; ++x) {
                if (x < 0 || x >= imgW) continue;
                float32 fx = (rx1 > rx0) ? float32(x - (curX + gOrd->x0 * scale8x)) / float32(gOrd->x1 * scale8x - gOrd->x0 * scale8x) : 0.f;
                float32 uOrd = gOrd->u0 + fx * (gOrd->u1 - gOrd->u0);
                float32 uSdf = gSdf->u0 + fx * (gSdf->u1 - gSdf->u0);

                // Échantillonnage bilinéaire entre 4 voisins
                float valOrd = SampleAtlasBilinear(pixOrd32, wOrd32, hOrd32, uOrd, vOrd);
                float valSdf = SampleAtlasBilinear(pixSdf32, wSdf32, hSdf32, uSdf, vSdf);

                // Couverture Ordinaire
                float alphaOrd = valOrd / 255.0f;
                uint8 rO = uint8(bgCol.r * (1.f - alphaOrd) + textCol.r * alphaOrd);
                uint8 gO = uint8(bgCol.g * (1.f - alphaOrd) + textCol.g * alphaOrd);
                uint8 bO = uint8(bgCol.b * (1.f - alphaOrd) + textCol.b * alphaOrd);
                imgOrd.SetPixel(x, y, math::NkColor(rO, gO, bO, 255));

                // Règle SDF avec smoothstep
                float dist = valSdf / 255.0f;
                const float s = 0.045f; // Douceur de transition sans effet d'escalier
                float alphaSdf = smoothstep(0.5f - s, 0.5f + s, dist);
                uint8 rS = uint8(bgCol.r * (1.f - alphaSdf) + textCol.r * alphaSdf);
                uint8 gS = uint8(bgCol.g * (1.f - alphaSdf) + textCol.g * alphaSdf);
                uint8 bS = uint8(bgCol.b * (1.f - alphaSdf) + textCol.b * alphaSdf);
                imgSdf.SetPixel(x, y, math::NkColor(rS, gS, bS, 255));
            }
        }
        curX += gOrd->advanceX * scale8x;
    }

    imgOrd.SavePNG("sign_ordinaire.png");
    imgSdf.SavePNG("sign_sdf.png");

    // 3. Rendu 3D de la scène
    const char *sdfEnv = getenv("USE_SDF");
    bool useSdf = (sdfEnv && strcmp(sdfEnv, "1") == 0);

    const char *distEnv = getenv("DISTANCE_MODE");
    bool mode50cm = (distEnv && strcmp(distEnv, "50cm") == 0);

    NkWindowConfig winCfg;
    winCfg.title = "Exo 8 — Le SDF mesure";
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

    NkTexHandle panTex = renderer->GetTextures()->Load(useSdf ? "sign_sdf.png" : "sign_ordinaire.png");
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

    const float32 signH = 0.20f;
    const float32 signW = signH * (float32(imgW) / float32(imgH));
    const float32 signT = 0.02f;
    const float32 doorX = -0.70f;
    const float32 wallZ = -2.00f;
    const float32 doorH = 2.00f;
    const float32 signY = doorH + 0.12f;
    const float32 signZ = wallZ + (signT * 0.5f) + 0.005f;

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
            camera.SetPosition({-0.70f, 2.20f, -1.50f});
        } else {
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

        // Sol, murs, porte, panneau
        drawBox({0.0f, -0.05f, 0.0f}, {4.0f, 0.10f, 4.0f}, {0.35f, 0.35f, 0.38f});
        drawBox({0.0f, 1.25f, -2.00f}, {4.0f, 2.50f, 0.10f}, {0.70f, 0.72f, 0.75f});
        drawBox({-2.0f, 1.25f, 0.0f}, {0.10f, 2.50f, 4.0f}, {0.68f, 0.70f, 0.72f});
        drawBox({doorX, 1.00f, wallZ + 0.015f}, {0.90f, doorH, 0.03f}, {0.45f, 0.28f, 0.15f});
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
