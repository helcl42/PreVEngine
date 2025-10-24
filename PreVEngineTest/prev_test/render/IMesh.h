#ifndef __IMESH_H__
#define __IMESH_H__

#include "VertexLayout.h"

namespace prev_test::render {
struct MeshPart {
    uint32_t firstVertexIndex;

    uint32_t firstIndicesIndex;

    uint32_t indicesCount;

    std::vector<glm::vec3> vertices;

    uint32_t materialIndex;

    MeshPart(const uint32_t indicesCnt, const std::vector<glm::vec3>& verts)
        : MeshPart(0, 0, indicesCnt, verts, 0)
    {
    }

    MeshPart(const uint32_t firstVertex, const uint32_t firstIndex, const uint32_t indicesCnt, const std::vector<glm::vec3>& verts, const uint32_t materialIdx)
        : firstVertexIndex(firstVertex)
        , firstIndicesIndex(firstIndex)
        , indicesCount(indicesCnt)
        , vertices(verts)
        , materialIndex(materialIdx)
    {
    }
};

struct MeshNode {
    std::vector<unsigned int> meshPartIndices;

    glm::mat4 transform{ 1.0f };

    std::vector<MeshNode> children;
};

class IMesh {
public:
    virtual const prev_test::render::VertexLayout& GetVertexLayout() const = 0;

    virtual const void* GetVertexData() const = 0;

    virtual uint32_t GerVerticesCount() const = 0;

    virtual const std::vector<uint32_t>& GetIndices() const = 0;

    virtual uint32_t GetIndicesCount() const = 0;

    virtual const std::vector<MeshPart>& GetMeshParts() const = 0;

    virtual const MeshNode& GetRootNode() const = 0;

public:
    virtual ~IMesh() = default;
};

enum class FlatMeshConstellation {
    ZERO_X = 0,
    ZERO_Y = 1,
    ZERO_Z = 2,
};
} // namespace prev_test::render

#endif // !__IMESH_H__
