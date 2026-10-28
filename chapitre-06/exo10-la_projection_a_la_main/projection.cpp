// =============================================================================
// Chapitre 6 -- Exercice 10 : La projection a la main
// =============================================================================
#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkEventSystem.h"
#include "NKEvent/NkWindowEvent.h"

#include "NKMath/NkAngle.h"
#include "NKMath/NkMat.h"
#include "NKMath/NkQuat.h"

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

#include <cmath>
#include <cstdio>
#include <cstdlib>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "ProjectionALaMain";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

namespace {

const float32 kPi = 3.14159265358979323846f;
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

NkMat4f Projection(float32 gauche, float32 droite,
                   float32 haut, float32 bas,
                   float32 nearPlane, float32 farPlane) {
    const float32 tanLeft = std::tan(gauche);
    const float32 tanRight = std::tan(droite);
    const float32 tanUp = std::tan(haut);
    const float32 tanDown = std::tan(bas);
    const float32 tanWidth = tanRight - tanLeft;
    const float32 tanHeight = tanUp - tanDown;

    NkMat4f result; // matrice nulle
    result.mat[0][0] = 2.f / tanWidth;
    result.mat[1][1] = 2.f / tanHeight;
    result.mat[2][0] = (tanRight + tanLeft) / tanWidth;
    result.mat[2][1] = (tanUp + tanDown) / tanHeight;
    result.mat[2][2] = (farPlane + nearPlane) / (nearPlane - farPlane);
    result.mat[2][3] = -1.f;
    result.mat[3][2] = 2.f * farPlane * nearPlane / (nearPlane - farPlane);
    return result;
}

uint64 EnvU64(const char *nom, uint64 defaut) {
    const char *t = std::getenv(nom);
    if (t == nullptr || *t == '\0') {
        return defaut;
    }
    return static_cast<uint64>(std::strtoull(t, nullptr, 10));
}

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

void ExecuterComparaisonProjection() {
    FILE *fOut = std::fopen("ecarts.txt", "w");

    // 1. Epreuve symetrique
    // 70 degres vertical, ratio 1280/720, near=0.05, far=100
    const float32 fovYDeg = 70.0f;
    const float32 ratio = 1280.0f / 720.0f;
    const float32 nearP = 0.05f;
    const float32 farP = 100.0f;

    const float32 hautSym = (fovYDeg * 0.5f) * (kPi / 180.0f);
    const float32 basSym = -hautSym;
    const float32 droiteSym = std::atan(ratio * std::tan(hautSym));
    const float32 gaucheSym = -droiteSym;

    const NkMat4f maProjSym = Projection(gaucheSym, droiteSym,
                                         hautSym, basSym,
                                         nearP, farP);
    const NkMat4f refProjSym = NkMat4f::Perspective(NkAngle(fovYDeg),
                                                    ratio, nearP, farP);

    float32 maxEcartSym = 0.0f;
    for (int row = 0; row < 4; ++row) {
        const float32 e0 = std::fabs(maProjSym.mat[0][row] -
                                     refProjSym.mat[0][row]);
        const float32 e1 = std::fabs(maProjSym.mat[1][row] -
                                     refProjSym.mat[1][row]);
        const float32 e2 = std::fabs(maProjSym.mat[2][row] -
                                     refProjSym.mat[2][row]);
        const float32 e3 = std::fabs(maProjSym.mat[3][row] -
                                     refProjSym.mat[3][row]);
        if (e0 > maxEcartSym) maxEcartSym = e0;
        if (e1 > maxEcartSym) maxEcartSym = e1;
        if (e2 > maxEcartSym) maxEcartSym = e2;
        if (e3 > maxEcartSym) maxEcartSym = e3;

        std::printf("symmetric, row %d : %.6f %.6f %.6f %.6f\n",
                    row, e0, e1, e2, e3);
        if (fOut != nullptr) {
            std::fprintf(fOut, "symmetric, row %d : %.6f %.6f %.6f %.6f\n",
                         row, e0, e1, e2, e3);
        }
    }

    // 2. Epreuve Quest 2 : left -52, right 45, up 48, down -50
    const float32 qGauche = -52.0f * (kPi / 180.0f);
    const float32 qDroite =  45.0f * (kPi / 180.0f);
    const float32 qHaut   =  48.0f * (kPi / 180.0f);
    const float32 qBas    = -50.0f * (kPi / 180.0f);

    const NkMat4f maProjQuest = Projection(qGauche, qDroite,
                                           qHaut, qBas,
                                           nearP, farP);

    NkCamera3D cam;
    cam.SetNearFar(nearP, farP);
    cam.SetFovAsym(qGauche, qDroite, qHaut, qBas);
    const NkMat4f refProjQuest = cam.GetProj();

    float32 maxEcartQuest = 0.0f;
    for (int row = 0; row < 4; ++row) {
        const float32 e0 = std::fabs(maProjQuest.mat[0][row] -
                                     refProjQuest.mat[0][row]);
        const float32 e1 = std::fabs(maProjQuest.mat[1][row] -
                                     refProjQuest.mat[1][row]);
        const float32 e2 = std::fabs(maProjQuest.mat[2][row] -
                                     refProjQuest.mat[2][row]);
        const float32 e3 = std::fabs(maProjQuest.mat[3][row] -
                                     refProjQuest.mat[3][row]);
        if (e0 > maxEcartQuest) maxEcartQuest = e0;
        if (e1 > maxEcartQuest) maxEcartQuest = e1;
        if (e2 > maxEcartQuest) maxEcartQuest = e2;
        if (e3 > maxEcartQuest) maxEcartQuest = e3;

        std::printf("quest, row %d : %.6f %.6f %.6f %.6f\n",
                    row, e0, e1, e2, e3);
        if (fOut != nullptr) {
            std::fprintf(fOut, "quest, row %d : %.6f %.6f %.6f %.6f\n",
                         row, e0, e1, e2, e3);
        }
    }

    std::printf("symmetric : largest gap %.6f\n", maxEcartSym);
    std::printf("quest : largest gap %.6f\n", maxEcartQuest);

    if (fOut != nullptr) {
        std::fprintf(fOut, "symmetric : largest gap %.6f\n", maxEcartSym);
        std::fprintf(fOut, "quest : largest gap %.6f\n", maxEcartQuest);
        std::fclose(fOut);
    }
}

} // namespace

int nkmain(const NkEntryState &state) {
    (void)state;

    ExecuterComparaisonProjection();

    if (EnvU64("MASALLE_BENCH_ONLY", 0) == 1 ||
        EnvU64("MASALLE_COMPUTE_ONLY", 0) == 1) {
        return 0;
    }

    NkWindowConfig cfg;
    cfg.title = "Chapitre 6 - La projection a la main";
    cfg.width = 1280;
    cfg.height = 720;
    cfg.centered = true;
    cfg.vsync = true;

    NkWindow fenetre(cfg);
    if (!fenetre.IsValid()) {
        std::printf("[Projection] la fenetre n'a pas pu s'ouvrir\n");
        return 1;
    }

    NkDeviceInitInfo devInit{};
    devInit.surface = fenetre.GetSurfaceDesc();
    devInit.width = fenetre.GetSize().x;
    devInit.height = fenetre.GetSize().y;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInit);
    if (device == nullptr) {
        std::printf("[Projection] aucune API graphique disponible\n");
        return 1;
    }

    NkRendererConfig cfgEcran =
        NkRendererConfig::For2D(devInit.api, devInit.width, devInit.height);
    NkRenderer *rEcran = NkRenderer::Create(device, cfgEcran);
    if (rEcran == nullptr || !rEcran->Initialize()) {
        std::printf("[Projection] renderer ecran non initialise\n");
        NkDeviceFactory::Destroy(device);
        return 1;
    }

    NkOffscreenDesc od;
    od.width = devInit.width;
    od.height = devInit.height;
    od.hasDepth = true;
    od.hdr = false;
    od.colorFmt = NkGPUFormat::NK_RGBA8_UNORM;
    od.readable = true;
    od.readback = true;
    od.name = "Projection_Cible";

    NkOffscreenTarget *cible = rEcran->CreateOffscreen(od);
    if (cible == nullptr) {
        std::printf("[Projection] cible refusee\n");
        NkRenderer::Destroy(rEcran);
        NkDeviceFactory::Destroy(device);
        return 1;
    }

    NkRendererConfig cfgSalle =
        NkRendererConfig::ForGame(devInit.api, devInit.width, devInit.height);
    cfgSalle.Disable(NK_SS_RENDER2D);
    cfgSalle.Disable(NK_SS_TEXT);
    cfgSalle.Disable(NK_SS_OVERLAY);
    cfgSalle.Enable(NK_SS_OFFSCREEN);

    NkRenderer *renderer = NkRenderer::Create(device, cfgSalle);
    if (renderer == nullptr || !renderer->Initialize()) {
        std::printf("[Projection] renderer salle non initialise\n");
        NkRenderer::Destroy(rEcran);
        NkDeviceFactory::Destroy(device);
        return 1;
    }
    renderer->SetFinalColorTarget(
        rEcran->GetTextures()->GetRHIHandle(cible->GetColorResult()));

    const uint64 imageDeSortie = EnvU64("MASALLE_EXIT", 0);
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

        if (rEcran->BeginFrame()) {
            NkICommandBuffer *cmd = rEcran->GetCmd();
            NkRender3D *r3d = renderer->GetRender3D();

            renderer->FlushGraphRebuilds();
            r3d->ResetFrame();
            if (auto *mc = renderer->GetMaterialCollection()) {
                mc->Upload();
            }

            NkCamera3DData cd;
            cd.position = { 0.30f, kYeux, 1.20f };
            cd.target = { -0.70f, 1.10f, -kDemi };
            cd.up = { 0.0f, 1.0f, 0.0f };
            cd.fovY = 70.0f;
            cd.aspect = static_cast<float32>(devInit.width) /
                        static_cast<float32>(devInit.height);
            cd.nearPlane = 0.05f;
            cd.farPlane = 100.0f;

            NkSceneContext sctx;
            sctx.camera = NkCamera3D(cd);
            sctx.time = static_cast<float32>(image) / 60.0f;
            r3d->SetIBLStrength(0.35f);

            NkLightDesc soleil;
            soleil.type = NkLightType::NK_DIRECTIONAL;
            soleil.direction = { -0.5f, -1.0f, -0.35f };
            soleil.color = { 1.0f, 0.96f, 0.88f };
            soleil.intensity = 3.0f;
            soleil.castShadow = false;
            sctx.lights.PushBack(soleil);

            r3d->BeginScene(sctx);
            SoumettreLaSalle(r3d, renderer->GetMeshSystem());

            if (auto *graph = renderer->GetRenderGraph()) {
                graph->Execute(cmd);
            }

            if (auto *r2d = rEcran->GetRender2D()) {
                r2d->DrawImage(cible->GetColorResult(),
                               { 0.0f, 0.0f,
                                 static_cast<float32>(devInit.width),
                                 static_cast<float32>(devInit.height) });
            }

            rEcran->Present();
            rEcran->EndFrame();
        }

        ++image;
        if (imageDeSortie != 0 && image >= imageDeSortie) {
            tourne = false;
        }
    }

    NkRenderer::Destroy(renderer);
    NkRenderer::Destroy(rEcran);
    NkDeviceFactory::Destroy(device);
    return 0;
}
