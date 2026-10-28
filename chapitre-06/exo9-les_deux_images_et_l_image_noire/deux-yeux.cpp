// =============================================================================
// Chapitre 6 -- Exercice 9 : Les deux images et l'image noire
// =============================================================================
#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkEventSystem.h"
#include "NKEvent/NkWindowEvent.h"

#include "NKMath/NkQuat.h"
#include "NKMath/NkAngle.h"
#include "NKMath/NkMat.h"
#include "NKImage/Core/NkImage.h"

#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Core/NkRenderGraph.h"
#include "NKRenderer/Core/NkTextureLibrary.h"
#include "NKRenderer/Materials/NkMaterialCollection.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Offscreen/NkOffscreenTarget.h"
#include "NKRenderer/Tools/Render2D/NkRender2D.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "DeuxYeux";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

namespace {

const float32 kPiece = 4.00f;
const float32 kPlafond = 2.50f;
const float32 kMur = 0.10f;
const float32 kPorteL = 0.90f;
const float32 kPorteH = 2.00f;
const float32 kFenL = 1.20f;
const float32 kFenH = 1.00f;
const float32 kFenY = 1.40f;
const float32 kTableL = 1.20f;
const float32 kTableP = 0.80f;
const float32 kTableH = 0.75f;
const float32 kYeux = 1.70f;
const float32 kDemi = kPiece * 0.5f;

const uint32 kCibleW = 640;
const uint32 kCibleH = 720;
const float32 kIpd = 0.063f; // 63 mm
const float32 kDemiEcart = kIpd * 0.5f;

void Pave(NkRender3D *r3d, NkMeshSystem *meshes, const NkVec3f &centre,
          const NkVec3f &taille, const NkVec3f &teinte,
          float32 rugosite = 1.0f) {
    NkDrawCall3D dc;
    dc.mesh = meshes->GetCube();
    dc.transform = NkMat4f::Translate(centre) * NkMat4f::Scale(taille);
    dc.aabb = { { centre.x - taille.x * 0.5f,
                  centre.y - taille.y * 0.5f,
                  centre.z - taille.z * 0.5f },
                { centre.x + taille.x * 0.5f,
                  centre.y + taille.y * 0.5f,
                  centre.z + taille.z * 0.5f } };
    dc.tint = teinte;
    dc.metallic = 0.0f;
    dc.roughness = rugosite;
    r3d->Submit(dc);
}

void SoumettreLaSalle(NkRender3D *r3d, NkMeshSystem *meshes) {
    const NkVec3f grisMur = { 0.72f, 0.70f, 0.66f };
    const NkVec3f grisSol = { 0.38f, 0.36f, 0.34f };
    const NkVec3f bois = { 0.55f, 0.38f, 0.22f };
    const NkVec3f vitre = { 0.75f, 0.85f, 0.92f };

    {
        NkDrawCall3D dc;
        dc.mesh = meshes->GetPlane();
        dc.transform = NkMat4f::Scale({ kPiece, 1.0f, kPiece });
        dc.aabb = { { -kDemi, 0.0f, -kDemi }, { kDemi, 0.0f, kDemi } };
        dc.castShadow = false;
        dc.tint = grisSol;
        dc.metallic = 0.0f;
        dc.roughness = 1.0f;
        r3d->Submit(dc);
    }

    const float32 yMur = kPlafond * 0.5f;
    const float32 bord = kDemi + kMur * 0.5f;

    Pave(r3d, meshes, { 0.0f, yMur, -bord },
         { kPiece, kPlafond, kMur }, grisMur);
    Pave(r3d, meshes, { 0.0f, yMur, bord },
         { kPiece, kPlafond, kMur }, grisMur);
    Pave(r3d, meshes, { -bord, yMur, 0.0f },
         { kMur, kPlafond, kPiece }, grisMur);
    Pave(r3d, meshes, { bord, yMur, 0.0f },
         { kMur, kPlafond, kPiece }, grisMur);

    Pave(r3d, meshes, { 0.0f, kPlafond + kMur * 0.5f, 0.0f },
         { kPiece, kMur, kPiece }, grisMur);

    Pave(r3d, meshes, { -0.70f, kPorteH * 0.5f, -kDemi + 0.02f },
         { kPorteL, kPorteH, 0.04f }, bois, 0.6f);

    Pave(r3d, meshes, { -kDemi + 0.02f, kFenY, -0.90f },
         { 0.04f, kFenH, kFenL }, vitre, 0.15f);
    Pave(r3d, meshes, { -kDemi + 0.02f, kFenY, 0.90f },
         { 0.04f, kFenH, kFenL }, vitre, 0.15f);

    const float32 tx = 0.60f, tz = 0.70f;
    const float32 ep = 0.05f;
    Pave(r3d, meshes, { tx, kTableH - ep * 0.5f, tz },
         { kTableL, ep, kTableP }, bois, 0.5f);

    const float32 pied = 0.06f;
    const float32 hp = kTableH - ep;
    for (int i = 0; i < 4; ++i) {
        const float32 dx = ((i & 1) ? 1.0f : -1.0f) *
                           (kTableL * 0.5f - pied);
        const float32 dz = ((i & 2) ? 1.0f : -1.0f) *
                           (kTableP * 0.5f - pied);
        Pave(r3d, meshes, { tx + dx, hp * 0.5f, tz + dz },
             { pied, hp, pied }, bois, 0.6f);
    }

    Pave(r3d, meshes, { -1.10f, 0.50f, 1.20f }, { 1.0f, 1.0f, 1.0f },
         { 0.85f, 0.80f, 0.30f }, 0.8f);
}

uint64 EnvU64(const char *nom, uint64 defaut) {
    const char *t = std::getenv(nom);
    if (t == nullptr || *t == '\0') {
        return defaut;
    }
    return static_cast<uint64>(std::strtoull(t, nullptr, 10));
}

void ComparerImages(const char *cheminA, const char *cheminB,
                    const char *cheminDiff,
                    int32 &outPixelsDiff, int32 &outTotal, int32 &outMaxGap) {
    NkImage imgA;
    NkImage imgB;
    if (!imgA.Load(cheminA) || !imgB.Load(cheminB)) {
        std::printf("[DeuxYeux] echec chargement %s ou %s\n",
                    cheminA, cheminB);
        outPixelsDiff = 0;
        outTotal = 0;
        outMaxGap = 0;
        return;
    }

    const int32 w = imgA.Width();
    const int32 h = imgA.Height();
    outTotal = w * h;
    outPixelsDiff = 0;
    outMaxGap = 0;

    NkImage imgDiff = NkImage::Create(static_cast<uint32>(w),
                                      static_cast<uint32>(h), 4);

    for (int32 y = 0; y < h; ++y) {
        for (int32 x = 0; x < w; ++x) {
            const math::NkColor ca = imgA.GetPixel(x, y);
            const math::NkColor cb = imgB.GetPixel(x, y);

            const int32 dr = std::abs(static_cast<int32>(ca.r) -
                                      static_cast<int32>(cb.r));
            const int32 dg = std::abs(static_cast<int32>(ca.g) -
                                      static_cast<int32>(cb.g));
            const int32 db = std::abs(static_cast<int32>(ca.b) -
                                      static_cast<int32>(cb.b));

            const int32 gap = std::max({ dr, dg, db });
            if (gap > 1) {
                ++outPixelsDiff;
            }
            if (gap > outMaxGap) {
                outMaxGap = gap;
            }

            const uint8 val = static_cast<uint8>(
                std::min(255, gap * 4));
            imgDiff.SetPixel(x, y, math::NkColor(val, val, val, 255));
        }
    }

    imgDiff.SavePNG(cheminDiff);
}

} // namespace

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig cfg;
    cfg.title = "Chapitre 6 - Les deux images et l'image noire";
    cfg.width = 1280;
    cfg.height = 720;
    cfg.centered = true;
    cfg.vsync = true;

    NkWindow fenetre(cfg);
    if (!fenetre.IsValid()) {
        std::printf("[DeuxYeux] la fenetre n'a pas pu s'ouvrir\n");
        return 1;
    }

    NkDeviceInitInfo devInit{};
    devInit.surface = fenetre.GetSurfaceDesc();
    devInit.width = fenetre.GetSize().x;
    devInit.height = fenetre.GetSize().y;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInit);
    if (device == nullptr) {
        std::printf("[DeuxYeux] aucune API graphique disponible\n");
        return 1;
    }

    NkRendererConfig cfgEcran =
        NkRendererConfig::For2D(devInit.api, devInit.width, devInit.height);
    NkRenderer *rEcran = NkRenderer::Create(device, cfgEcran);
    if (rEcran == nullptr || !rEcran->Initialize()) {
        std::printf("[DeuxYeux] renderer ecran non initialise\n");
        NkDeviceFactory::Destroy(device);
        return 1;
    }

    // Deux cibles hors-ecran : une par oeil (640 x 720)
    NkOffscreenDesc odG;
    odG.width = kCibleW;
    odG.height = kCibleH;
    odG.hasDepth = true;
    odG.hdr = false;
    odG.colorFmt = NkGPUFormat::NK_RGBA8_UNORM;
    odG.readable = true;
    odG.readback = true;
    odG.name = "CibleGauche";

    NkOffscreenTarget *cibleG = rEcran->CreateOffscreen(odG);
    if (cibleG == nullptr) {
        std::printf("[DeuxYeux] cible gauche refusee\n");
        NkRenderer::Destroy(rEcran);
        NkDeviceFactory::Destroy(device);
        return 1;
    }

    NkOffscreenDesc odD = odG;
    odD.name = "CibleDroite";
    NkOffscreenTarget *cibleD = rEcran->CreateOffscreen(odD);
    if (cibleD == nullptr) {
        std::printf("[DeuxYeux] cible droite refusee\n");
        NkRenderer::Destroy(rEcran);
        NkDeviceFactory::Destroy(device);
        return 1;
    }

    // Deux renderers : un par oeil
    NkRendererConfig cfgOeil =
        NkRendererConfig::ForGame(devInit.api, kCibleW, kCibleH);
    cfgOeil.Disable(NK_SS_RENDER2D);
    cfgOeil.Disable(NK_SS_TEXT);
    cfgOeil.Disable(NK_SS_OVERLAY);
    cfgOeil.Enable(NK_SS_OFFSCREEN);

    NkRenderer *rG = NkRenderer::Create(device, cfgOeil);
    NkRenderer *rD = NkRenderer::Create(device, cfgOeil);
    if (rG == nullptr || !rG->Initialize() ||
        rD == nullptr || !rD->Initialize()) {
        std::printf("[DeuxYeux] echec initialisation renderers oeil\n");
        NkRenderer::Destroy(rEcran);
        NkDeviceFactory::Destroy(device);
        return 1;
    }

    rG->SetFinalColorTarget(
        rEcran->GetTextures()->GetRHIHandle(cibleG->GetColorResult()));
    rG->SetRenderSizeOverride(kCibleW, kCibleH);

    rD->SetFinalColorTarget(
        rEcran->GetTextures()->GetRHIHandle(cibleD->GetColorResult()));
    rD->SetRenderSizeOverride(kCibleW, kCibleH);

    const uint64 imageDeSortie = EnvU64("MASALLE_EXIT", 45);
    uint64 image = 0;
    bool tourne = true;

    while (tourne && fenetre.IsOpen()) {
        NkEvent *ev = nullptr;
        while (NkEvents().PollEvent(ev)) {
            if (ev->As<NkWindowCloseEvent>() != nullptr) {
                tourne = false;
            }
        }
        if (!tourne) {
            break;
        }

        // Vue stereo jusqu'a l'image 32, puis vue oubliee sans ecart
        const bool vueOubliee = (image >= 31);

        if (rEcran->BeginFrame()) {
            NkICommandBuffer *cmd = rEcran->GetCmd();

            // Poses de camera
            const NkVec3f posTete = { 0.30f, kYeux, 1.20f };
            const NkVec3f dirRegard = { 0.0f, 0.0f, -1.0f };
            const NkVec3f upTete = { 0.0f, 1.0f, 0.0f };

            NkCamera3DData cdG;
            cdG.position = vueOubliee ? posTete
                                      : (posTete - NkVec3f(kDemiEcart, 0, 0));
            cdG.target = cdG.position + dirRegard * 5.0f;
            cdG.up = upTete;
            cdG.fovY = 70.0f;
            cdG.aspect = static_cast<float32>(kCibleW) /
                         static_cast<float32>(kCibleH);
            cdG.nearPlane = 0.05f;
            cdG.farPlane = 100.0f;

            NkCamera3DData cdD = cdG;
            cdD.position = vueOubliee ? posTete
                                      : (posTete + NkVec3f(kDemiEcart, 0, 0));
            cdD.target = cdD.position + dirRegard * 5.0f;

            // Rendu Oeil Gauche
            {
                NkRender3D *r3dG = rG->GetRender3D();
                rG->FlushGraphRebuilds();
                r3dG->ResetFrame();
                if (auto *mc = rG->GetMaterialCollection()) {
                    mc->Upload();
                }

                NkSceneContext sctxG;
                sctxG.camera = NkCamera3D(cdG);
                sctxG.time = static_cast<float32>(image) / 60.0f;
                r3dG->SetIBLStrength(0.35f);

                NkLightDesc soleil;
                soleil.type = NkLightType::NK_DIRECTIONAL;
                soleil.direction = { -0.5f, -1.0f, -0.35f };
                soleil.color = { 1.0f, 0.96f, 0.88f };
                soleil.intensity = 3.0f;
                soleil.castShadow = false;
                sctxG.lights.PushBack(soleil);

                r3dG->BeginScene(sctxG);
                SoumettreLaSalle(r3dG, rG->GetMeshSystem());

                if (auto *graph = rG->GetRenderGraph()) {
                    graph->Execute(cmd);
                }
            }

            // Rendu Oeil Droit
            {
                NkRender3D *r3dD = rD->GetRender3D();
                rD->FlushGraphRebuilds();
                r3dD->ResetFrame();
                if (auto *mc = rD->GetMaterialCollection()) {
                    mc->Upload();
                }

                NkSceneContext sctxD;
                sctxD.camera = NkCamera3D(cdD);
                sctxD.time = static_cast<float32>(image) / 60.0f;
                r3dD->SetIBLStrength(0.35f);

                NkLightDesc soleil;
                soleil.type = NkLightType::NK_DIRECTIONAL;
                soleil.direction = { -0.5f, -1.0f, -0.35f };
                soleil.color = { 1.0f, 0.96f, 0.88f };
                soleil.intensity = 3.0f;
                soleil.castShadow = false;
                sctxD.lights.PushBack(soleil);

                r3dD->BeginScene(sctxD);
                SoumettreLaSalle(r3dD, rD->GetMeshSystem());

                if (auto *graph = rD->GetRenderGraph()) {
                    graph->Execute(cmd);
                }
            }

            // Affichage cote a cote
            if (auto *r2d = rEcran->GetRender2D()) {
                r2d->DrawImage(cibleG->GetColorResult(),
                               { 0.0f, 0.0f,
                                 static_cast<float32>(kCibleW),
                                 static_cast<float32>(kCibleH) });
                r2d->DrawImage(cibleD->GetColorResult(),
                               { static_cast<float32>(kCibleW), 0.0f,
                                 static_cast<float32>(kCibleW),
                                 static_cast<float32>(kCibleH) });
            }

            rEcran->Present();
            rEcran->EndFrame();
        }

        ++image;

        // Captures a frame 30 (deux yeux avec ecart stereo)
        if (image == 30) {
            cibleG->Capture("oeil-gauche.png");
            cibleD->Capture("oeil-droit.png");
        }

        // Captures a frame 40 (vue oubliee, deux yeux sans ecart)
        if (image == 40) {
            cibleG->Capture("oeil-gauche-oublie.png");
            cibleD->Capture("oeil-droit-oublie.png");
        }

        if (imageDeSortie != 0 && image >= imageDeSortie) {
            tourne = false;
        }
    }

    NkRenderer::Destroy(rG);
    NkRenderer::Destroy(rD);
    NkRenderer::Destroy(rEcran);
    NkDeviceFactory::Destroy(device);

    // Post-traitement et soustraction des images
    int32 diffYeux = 0, totYeux = 0, maxGapYeux = 0;
    ComparerImages("oeil-gauche.png", "oeil-droit.png",
                   "difference.png",
                   diffYeux, totYeux, maxGapYeux);

    int32 diffOubli = 0, totOubli = 0, maxGapOubli = 0;
    ComparerImages("oeil-gauche-oublie.png", "oeil-droit-oublie.png",
                   "difference-vue-oubliee.png",
                   diffOubli, totOubli, maxGapOubli);

    FILE *f = std::fopen("difference.txt", "w");
    if (f != nullptr) {
        std::fprintf(f, "two eyes : %d pixels out of %d, max gap %d\n",
                     diffYeux, totYeux, maxGapYeux);
        std::fprintf(f, "view forgotten : %d pixels out of %d, max gap %d\n",
                     diffOubli, totOubli, maxGapOubli);
        std::fclose(f);
    }

    std::printf("two eyes : %d pixels out of %d, max gap %d\n",
                diffYeux, totYeux, maxGapYeux);
    std::printf("view forgotten : %d pixels out of %d, max gap %d\n",
                diffOubli, totOubli, maxGapOubli);

    return 0;
}
