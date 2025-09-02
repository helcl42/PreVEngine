#ifndef __PLANE_MESH_H__
#define __PLANE_MESH_H__

#include "../IMesh.h"
#include "../VertexDataBuffer.h"

namespace prev_test::render::mesh {
class PlaneMesh final : public prev_test::render::IMesh {
public:
    PlaneMesh(const float xSize, const float zSize, const uint32_t xDivs, const uint32_t zDivs, const float textureCoordUMax = 1.0f, const float textureCoordVMax = 1.0f, const FlatMeshConstellation constellation = FlatMeshConstellation::ZERO_Y, const bool generateTangentBiTangent = false);

    ~PlaneMesh() = default;

public:
    const prev_test::render::VertexLayout& GetVertexLayout() const override;

    const void* GetVertexData() const override;

    uint32_t GerVerticesCount() const override;

    const std::vector<uint32_t>& GetIndices() const override;

    uint32_t GetIndicesCount() const override;

    const std::vector<prev_test::render::MeshPart>& GetMeshParts() const override;

    const MeshNode& GetRootNode() const override;

private:
    prev_test::render::VertexLayout m_vertexLayout;

    prev_test::render::VertexDataBuffer m_vertexDataBuffer;

    std::vector<glm::vec3> m_vertices;

    std::vector<uint32_t> m_indices;

    std::vector<prev_test::render::MeshPart> m_meshParts;

    prev_test::render::MeshNode m_meshRootNode;
};
} // namespace prev_test::render::mesh

#endif // !__PLANE_MESH_H__
