// =============================================================================
// Chapitre 6 -- Exercice 5 : Les deux ordres et la matrice
// =============================================================================
#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkEventSystem.h"
#include "NKEvent/NkWindowEvent.h"

#include "NKMath/NkQuat.h"
#include "NKMath/NkAngle.h"
#include "NKMath/NkMat.h"

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
    d.appName = "DeuxOrdresEtMatrice";
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

} // namespace

int nkmain(const NkEntryState &state) {
    (void)state;

    // Calculs d'ordres de quaternions et matrices au demarrage
    const NkQuatf qLacet = NkQuatf::RotateY(NkAngle(45.0f));
    const NkQuatf qTangage = NkQuatf::RotateX(NkAngle(30.0f));
    const NkVec3f avant = { 0.0f, 0.0f, -1.0f };

    const NkQuatf qYP = qLacet * qTangage;
    const NkQuatf qPY = qTangage * qLacet;

    const NkMat4f rotMat = NkMat4f::RotationY(NkAngle(45.0f)) *
                           NkMat4f::RotationX(NkAngle(30.0f));
    const NkQuatf qMat = NkQuatf(rotMat).Normalized();

    const NkVec3f regardYP = qYP * avant;
    const NkVec3f regardPY = qPY * avant;
    const NkVec3f regardMat = qMat * avant;

    std::printf("yaw*pitch : (%.4f, %.4f, %.4f)\n",
                static_cast<double>(regardYP.x),
                static_cast<double>(regardYP.y),
                static_cast<double>(regardYP.z));
    std::printf("pitch*yaw : (%.4f, %.4f, %.4f)\n",
                static_cast<double>(regardPY.x),
                static_cast<double>(regardPY.y),
                static_cast<double>(regardPY.z));
    std::printf("matrices : (%.4f, %.4f, %.4f)\n",
                static_cast<double>(regardMat.x),
                static_cast<double>(regardMat.y),
                static_cast<double>(regardMat.z));

    // Calcul a la main de la composante Y pour tangage = 30 deg :
    // Une tete levee de 30 degres possede une composante Y egale a sin(30 deg)
    // = 0.5000. La rotation en lacet autour de l'axe vertical Y ne modifie
    // jamais la coordonnee Y d'un vecteur. On attend donc
    // strictement Y = 0.5000.
    const float32 yAttenduMain = std::sin(30.0f * 3.1415926535f / 180.0f);
    std::printf("calcul main Y : %.4f (compare: YP=%.4f, PY=%.4f, Mat=%.4f)\n",
                static_cast<double>(yAttenduMain),
                static_cast<double>(regardYP.y),
                static_cast<double>(regardPY.y),
                static_cast<double>(regardMat.y));
    std::fflush(stdout);

    NkWindowConfig cfg;
    cfg.title = "Chapitre 6 - Les deux ordres et la matrice";
    cfg.width = 1280;
    cfg.height = 720;
    cfg.centered = true;
    cfg.vsync = true;

    NkWindow fenetre(cfg);
    if (!fenetre.IsValid()) {
        std::printf("[Ordres] la fenetre n'a pas pu s'ouvrir\n");
        return 1;
    }

    NkDeviceInitInfo devInit{};
    devInit.surface = fenetre.GetSurfaceDesc();
    devInit.width = fenetre.GetSize().x;
    devInit.height = fenetre.GetSize().y;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInit);
    if (device == nullptr) {
        std::printf("[Ordres] aucune API graphique disponible\n");
        return 1;
    }

    NkRendererConfig cfgEcran =
        NkRendererConfig::For2D(devInit.api, devInit.width, devInit.height);
    NkRenderer *rEcran = NkRenderer::Create(device, cfgEcran);
    if (rEcran == nullptr || !rEcran->Initialize()) {
        std::printf("[Ordres] renderer ecran non initialise\n");
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
    od.name = "Ordres_Cible";

    NkOffscreenTarget *cible = rEcran->CreateOffscreen(od);
    if (cible == nullptr) {
        std::printf("[Ordres] cible refusee\n");
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

    if (EnvU64("MASALLE_POST", 1) == 0) {
        cfgSalle.Disable(NK_SS_POST_PROCESS);
    }

    NkRenderer *renderer = NkRenderer::Create(device, cfgSalle);
    if (renderer == nullptr || !renderer->Initialize()) {
        std::printf("[Ordres] renderer salle non initialise\n");
        NkRenderer::Destroy(rEcran);
        NkDeviceFactory::Destroy(device);
        return 1;
    }
    renderer->SetFinalColorTarget(
        rEcran->GetTextures()->GetRHIHandle(cible->GetColorResult()));

    const uint64 imageDeSortie = EnvU64("MASALLE_EXIT", 0);
    const uint64 imageDeCapture = EnvU64("MASALLE_SHOT", 0);

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
            cd.target = cd.position + regardYP;
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

        if (imageDeCapture != 0 && image == imageDeCapture) {
            const char *chemin = std::getenv("MASALLE_SHOT_PATH");
            if (chemin == nullptr || *chemin == '\0') {
                chemin = "masalle.png";
            }
            cible->Capture(chemin);
        }

        if (imageDeSortie != 0 && image >= imageDeSortie) {
            tourne = false;
        }
    }

    NkRenderer::Destroy(renderer);
    NkRenderer::Destroy(rEcran);
    NkDeviceFactory::Destroy(device);
    return 0;
}
