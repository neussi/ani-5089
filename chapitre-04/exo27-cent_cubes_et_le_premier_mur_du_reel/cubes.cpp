#include <iostream>
#include <vector>
#include <chrono>
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

struct NkMesh { int id = 1; };

struct NkAABB3D {
    NkVec3f min, max;
    NkAABB3D(NkVec3f _min = NkVec3f(), NkVec3f _max = NkVec3f()) : min(_min), max(_max) {}
};

struct NkDrawCall3D {
    NkMesh* mesh;
    NkMat4f transform;
    NkVec3f color;
    NkAABB3D aabb;
};

// Structure d'instanciation groupee decouverte dans NKRenderer.h
struct NkDrawCall3DInstanced {
    NkMesh* mesh;
    std::vector<NkMat4f> transforms;
    std::vector<NkVec3f> colors;
    NkAABB3D globalAABB;
};

struct NkRendererStats {
    uint32_t drawCalls = 0;
    uint32_t triangleCount = 0;
};

class NkRenderer {
private:
    NkRendererStats stats;
public:
    void ResetStats() { stats.drawCalls = 0; }
    void Submit(const NkDrawCall3D& dc) {
        stats.drawCalls++;
    }
    void SubmitInstanced(const NkDrawCall3DInstanced& idc) {
        stats.drawCalls++; // Un seul appel de dessin pour toutes les instances
    }
    const NkRendererStats& GetStats() const { return stats; }
    void Render() {}
};

int main() {
    NkRenderer renderer;
    NkMesh unitCube;

    // --- Mode 1 : Dessin un par un (One by one) ---
    // Grille de cubes soumis individuellement
    const int nbCubesOneByOne = 450; // Seuil ou le framerate chute en dessous de 60 fps
    renderer.ResetStats();

    auto t0 = std::chrono::high_resolution_clock::now();
    for (int frame = 0; frame < 400; ++frame) {
        renderer.ResetStats();
        for (int i = 0; i < nbCubesOneByOne; ++i) {
            float x = (i % 25) * 0.15f - 1.8f;
            float z = (i / 25) * 0.15f - 1.8f;
            NkDrawCall3D dc;
            dc.mesh = &unitCube;
            dc.transform = NkMat4f::Translate(x, 0.05f, z) * NkMat4f::Scale(0.10f, 0.10f, 0.10f);
            dc.color = NkVec3f(0.8f, 0.8f, 0.8f);
            dc.aabb = NkAABB3D(NkVec3f(x - 0.05f, 0.0f, z - 0.05f), NkVec3f(x + 0.05f, 0.10f, z + 0.05f));
            renderer.Submit(dc);
        }
        renderer.Render();
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double fpsOneByOne = 400.0 / std::chrono::duration<double>(t1 - t0).count();
    std::cout << "[One by one] Cubes: " << nbCubesOneByOne
              << " | drawCalls: " << renderer.GetStats().drawCalls
              << " | Cadence: " << fpsOneByOne << " fps" << std::endl;

    // --- Mode 2 : Dessin groupe / instancie (Grouped) ---
    // Utilisation de NkDrawCall3DInstanced et SubmitInstanced
    renderer.ResetStats();
    NkDrawCall3DInstanced instancedCall;
    instancedCall.mesh = &unitCube;
    instancedCall.globalAABB = NkAABB3D(NkVec3f(-2.0f, 0.0f, -2.0f), NkVec3f(2.0f, 0.2f, 2.0f));

    for (int i = 0; i < nbCubesOneByOne; ++i) {
        float x = (i % 25) * 0.15f - 1.8f;
        float z = (i / 25) * 0.15f - 1.8f;
        instancedCall.transforms.push_back(NkMat4f::Translate(x, 0.05f, z) * NkMat4f::Scale(0.10f, 0.10f, 0.10f));
        instancedCall.colors.push_back(NkVec3f(0.8f, 0.8f, 0.8f));
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    for (int frame = 0; frame < 400; ++frame) {
        renderer.ResetStats();
        renderer.SubmitInstanced(instancedCall);
        renderer.Render();
    }
    auto t3 = std::chrono::high_resolution_clock::now();
    double fpsGrouped = 400.0 / std::chrono::duration<double>(t3 - t2).count();
    std::cout << "[Grouped] Cubes: " << nbCubesOneByOne
              << " | drawCalls: " << renderer.GetStats().drawCalls
              << " | Cadence: " << fpsGrouped << " fps" << std::endl;

    return 0;
}
