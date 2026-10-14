#include <iostream>
#include <cmath>
#include <cstdint>

// Structures et définitions du framework NK
struct NkVec3f {
    float x, y, z;
    NkVec3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) : x(_x), y(_y), z(_z) {}
    NkVec3f operator+(const NkVec3f& o) const { return NkVec3f(x + o.x, y + o.y, z + o.z); }
    NkVec3f operator-(const NkVec3f& o) const { return NkVec3f(x - o.x, y - o.y, z - o.z); }
    NkVec3f operator*(float s) const { return NkVec3f(x * s, y * s, z * s); }
    NkVec3f Normalized() const {
        float len = std::sqrt(x * x + y * y + z * z);
        return len > 0.0001f ? NkVec3f(x / len, y / len, z / len) : *this;
    }
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
    static NkMat4f Translate(const NkVec3f& p) {
        return Translate(p.x, p.y, p.z);
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

struct NkAABB3D {
    NkVec3f min;
    NkVec3f max;
    NkAABB3D(const NkVec3f& _min = NkVec3f(), const NkVec3f& _max = NkVec3f()) : min(_min), max(_max) {}
};

enum NkSubsystemFlags {
    NK_SS_NONE = 0,
    NK_SS_RENDER3D = (1 << 0),
    NK_SS_SHADOW = (1 << 1),
    NK_SS_UI2D = (1 << 2)
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

struct NkMesh {
    int id;
};

class NkMeshSystem {
public:
    NkMesh* GetCube() {
        static NkMesh unitCube = { 1 };
        return &unitCube;
    }
};

struct NkDrawCall3D {
    NkMesh* mesh;
    NkMat4f transform;
    NkVec3f color;
    NkAABB3D aabb;
};

struct NkCamera {
    NkVec3f position;
    NkVec3f target;
    NkVec3f up;
    float fovY;
    float nearPlane;
    float farPlane;
};

struct NkRendererConfig {
    uint32_t subsystems;
};

class NkRenderer {
public:
    void Init(const NkRendererConfig& config) {
        std::cout << "Renderer initialise avec les sous-systemes: " << config.subsystems << std::endl;
    }
    void SetCamera(const NkCamera& cam) {}
    void AddLight(const NkLight& light) {}
    void Submit(const NkDrawCall3D& dc) {
        std::cout << "Cube soumis au renderer avec succes." << std::endl;
    }
    void Render() {}
};

class NkDevice {
public:
    bool IsValid() const { return true; }
};

class NkDeviceFactory {
public:
    static NkDevice* CreateAutoDetect() {
        return new NkDevice();
    }
};

int main() {
    // 1. Creation du device
    NkDevice* device = NkDeviceFactory::CreateAutoDetect();
    if (!device || !device->IsValid()) {
        std::cerr << "Erreur de creation du device" << std::endl;
        return -1;
    }

    // 2. Creation du renderer en nommant explicitement ses sous-systemes
    NkRendererConfig config = {};
    config.subsystems = NK_SS_RENDER3D; // On allume uniquement le dessin de maillage 3D
    NkRenderer* renderer = new NkRenderer();
    renderer->Init(config);

    // 3. Placement de la camera a l'entree
    NkCamera camera;
    camera.position = NkVec3f(0.0f, 1.70f, 1.5f);
    camera.target   = NkVec3f(0.0f, 1.40f, -2.0f);
    camera.up       = NkVec3f(0.0f, 1.0f, 0.0f);
    camera.fovY     = 70.0f;
    camera.nearPlane = 0.05f;
    camera.farPlane  = 200.0f;
    renderer->SetCamera(camera);

    // 4. Declaration du soleil (lumiere directionnelle)
    NkLight sun;
    sun.type = NkLightType::NK_DIRECTIONAL;
    sun.direction = NkVec3f(-0.4f, -1.0f, -0.3f);
    sun.color = NkVec3f(1.0f, 0.95f, 0.85f);
    sun.intensity = 3.0f;
    sun.castShadow = false;
    renderer->AddLight(sun);

    // 5. Preparation du cube a deux metres devant la camera
    NkMeshSystem* meshSystem = new NkMeshSystem();
    NkVec3f forward = (camera.target - camera.position).Normalized();
    NkVec3f cubePos = camera.position + forward * 2.0f; // Exactement deux metres devant la camera

    NkMat4f cubeTransform = NkMat4f::Translate(cubePos) * NkMat4f::Scale(1.0f, 1.0f, 1.0f);

    NkDrawCall3D cubeDrawCall;
    cubeDrawCall.mesh = meshSystem->GetCube();
    cubeDrawCall.transform = cubeTransform;
    cubeDrawCall.color = NkVec3f(1.0f, 1.0f, 1.0f); // Teinte blanche
    cubeDrawCall.aabb = NkAABB3D(
        cubePos - NkVec3f(0.5f, 0.5f, 0.5f),
        cubePos + NkVec3f(0.5f, 0.5f, 0.5f)
    );

    // Soumission du cube
    renderer->Submit(cubeDrawCall);

    renderer->Render();
    return 0;
}
