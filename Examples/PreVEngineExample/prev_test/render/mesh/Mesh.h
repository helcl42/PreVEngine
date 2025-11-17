#ifndef __MESH_H__
#define __MESH_H__

#include "../IMesh.h"
#include "../VertexDataBuffer.h"

namespace prev_test::render::mesh {
class Mesh final : public prev_test::render::IMesh {
public:
    Mesh(const prev_test::render::VertexLayout& vertexLayout, const prev_test::render::VertexDataBuffer& vertexDataBuffer, const std::vector<uint32_t>& indices, const prev_test::render::MeshNode& meshRootNode, const std::vector<prev_test::render::MeshPart>& meshParts);

    ~Mesh() = default;

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

    std::vector<uint32_t> m_indices;

    prev_test::render::MeshNode m_meshRootNode;

    std::vector<prev_test::render::MeshPart> m_meshParts;
};
} // namespace prev_test::render::mesh

#endif // !__MESH_H__