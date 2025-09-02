#ifndef __QUAD_MESH_H__
#define __QUAD_MESH_H__

#include "../IMesh.h"
#include "../VertexDataBuffer.h"

namespace prev_test::render::mesh {
class QuadMesh final : public IMesh {
public:
    QuadMesh(const FlatMeshConstellation constellation, const bool includeTangentBiTangent = false);

    ~QuadMesh() = default;

public:
    const prev_test::render::VertexLayout& GetVertexLayout() const override;

    const void* GetVertexData() const override;

    uint32_t GerVerticesCount() const override;

    const std::vector<uint32_t>& GetIndices() const override;

    uint32_t GetIndicesCount() const override;

    const std::vector<MeshPart>& GetMeshParts() const override;

    const MeshNode& GetRootNode() const override;

private:
    prev_test::render::VertexLayout m_vertexLayout;

    prev_test::render::VertexDataBuffer m_vertexDataBuffer;

private:
    std::vector<glm::vec3> m_vertices;

    std::vector<uint32_t> m_indices;

    std::vector<MeshPart> m_meshParts;

    prev_test::render::MeshNode m_meshRootNode;
};
} // namespace prev_test::render::mesh

#endif // !__QUAD_MESH_H__
