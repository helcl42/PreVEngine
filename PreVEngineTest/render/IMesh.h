#ifndef __IMESH_H__
#define __IMESH_H__

#include "VertexLayout.h"

namespace prev_test::render {
struct MeshPart {
    uint32_t firstVertexIndex;

    uint32_t firstIndicesIndex;

    uint32_t indicesCount;

    uint32_t materialIndex;

    MeshPart(const uint32_t indicesCnt)
        : MeshPart(0, 0, indicesCnt, 0)
    {
    }

    MeshPart(const uint32_t firstVertex, const uint32_t firstIndex, const uint32_t indicesCnt, const uint32_t materialIdx)
        : firstVertexIndex(firstVertex)
        , firstIndicesIndex(firstIndex)
        , indicesCount(indicesCnt)
        , materialIndex(materialIdx)
    {
    }
};

struct MeshNode {
    std::vector<unsigned int> meshPartIndices;

    glm::mat4 transform;

    std::vector<MeshNode> children;

    MeshNode() = default;
};

class IMesh {
public:
    virtual const prev_test::render::VertexLayout& GetVertexLayout() const = 0;

    virtual const void* GetVertexData() const = 0;

    virtual const std::vector<glm::vec3>& GetVertices() const = 0;

    virtual uint32_t GerVerticesCount() const = 0;

    virtual const std::vector<uint32_t>& GetIndices() const = 0;

    virtual const std::vector<MeshPart>& GetMeshParts() const = 0;

    virtual const MeshNode& GetRootNode() const = 0;

public:
    virtual ~IMesh() = default;
};
} // namespace prev_test::render

#endif // !__IMESH_H__
