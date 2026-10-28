// =============================================================================
// Chapitre 6 -- Exercice 8 : Les deux poses et l'axe du monde
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
    d.appName = "LesDeuxPoses";
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

struct PoseOeil {
    NkVec3f position;
};

void ExecuterCalculPoses() {
    const NkVec3f positionTete = { 0.30f, 1.70f, 1.20f };
    const float32 ipd = 0.063f; // 63 mm
    const float32 demiEcart = ipd * 0.5f;

    struct Epreuve {
        const char *nom;
        NkQuatf orientation;
    };

    Epreuve poses[3] = {
        { "neutral", NkQuatf::Identity() },
        { "roll 30", NkQuatf::RotateZ(NkAngle(30.f)) },
        { "yaw 90",  NkQuatf::RotateY(NkAngle(90.f)) }
    };

    float32 ecartsMm[3];

    for (int i = 0; i < 3; ++i) {
        const char *nom = poses[i].nom;
        const NkQuatf &q = poses[i].orientation;

        // Droite de la tete : image de +X du monde a travers l'orientation
        const NkVec3f droiteTete = q * NkVec3f(1.0f, 0.0f, 0.0f);
        const NkVec3f droiteMonde = { 1.0f, 0.0f, 0.0f };

        // Methode 1 : selon la droite de la tete
        PoseOeil gTete, dTete;
        gTete.position = positionTete - droiteTete * demiEcart;
        dTete.position = positionTete + droiteTete * demiEcart;

        // Methode 2 : selon l'axe X du monde
        PoseOeil gMonde, dMonde;
        gMonde.position = positionTete - droiteMonde * demiEcart;
        dMonde.position = positionTete + droiteMonde * demiEcart;

        std::printf("%s, head : L (%.4f, %.4f, %.4f) R (%.4f, %.4f, %.4f)\n",
                    nom,
                    gTete.position.x, gTete.position.y, gTete.position.z,
                    dTete.position.x, dTete.position.y, dTete.position.z);
        std::printf("%s, world : L (%.4f, %.4f, %.4f) R (%.4f, %.4f, %.4f)\n",
                    nom,
                    gMonde.position.x, gMonde.position.y, gMonde.position.z,
                    dMonde.position.x, dMonde.position.y, dMonde.position.z);

        const NkVec3f diff = gTete.position - gMonde.position;
        const float32 distMetres = std::sqrt(diff.x * diff.x +
                                             diff.y * diff.y +
                                             diff.z * diff.z);
        ecartsMm[i] = distMetres * 1000.0f;
    }

    for (int i = 0; i < 3; ++i) {
        std::printf("%s : gap %.1f mm\n", poses[i].nom, ecartsMm[i]);
    }
}

} // namespace

int nkmain(const NkEntryState &state) {
    (void)state;

    ExecuterCalculPoses();

    if (EnvU64("MASALLE_BENCH_ONLY", 0) == 1 ||
        EnvU64("MASALLE_COMPUTE_ONLY", 0) == 1) {
        return 0;
    }

    NkWindowConfig cfg;
    cfg.title = "Chapitre 6 - Les deux poses et l'axe du monde";
    cfg.width = 1280;
    cfg.height = 720;
    cfg.centered = true;
    cfg.vsync = true;

    NkWindow fenetre(cfg);
    if (!fenetre.IsValid()) {
        std::printf("[Poses] la fenetre n'a pas pu s'ouvrir\n");
        return 1;
    }

    NkDeviceInitInfo devInit{};
    devInit.surface = fenetre.GetSurfaceDesc();
    devInit.width = fenetre.GetSize().x;
    devInit.height = fenetre.GetSize().y;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInit);
    if (device == nullptr) {
        std::printf("[Poses] aucune API graphique disponible\n");
        return 1;
    }

    NkRendererConfig cfgEcran =
        NkRendererConfig::For2D(devInit.api, devInit.width, devInit.height);
    NkRenderer *rEcran = NkRenderer::Create(device, cfgEcran);
    if (rEcran == nullptr || !rEcran->Initialize()) {
        std::printf("[Poses] renderer ecran non initialise\n");
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
    od.name = "Poses_Cible";

    NkOffscreenTarget *cible = rEcran->CreateOffscreen(od);
    if (cible == nullptr) {
        std::printf("[Poses] cible refusee\n");
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
        std::printf("[Poses] renderer salle non initialise\n");
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
