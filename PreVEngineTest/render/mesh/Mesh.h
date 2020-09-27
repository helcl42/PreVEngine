#ifndef __MESH_H__
#define __MESH_H__

#include "../IMesh.h"
#include "../VertexDataBuffer.h"

namespace prev_test::render::mesh {
class MeshFactory;

class Mesh final : public prev_test::render::IMesh {
public:
    const prev_test::render::VertexLayout& GetVertexLayout() const override;

    const void* GetVertexData() const override;

    const std::vector<glm::vec3>& GetVertices() const override;

    uint32_t GerVerticesCount() const override;

    const std::vector<uint32_t>& GetIndices() const override;

    const std::vector<prev_test::render::MeshPart>& GetMeshParts() const override;

private:
    friend MeshFactory;

private:
    prev_test::render::VertexLayout m_vertexLayout;

    prev_test::render::VertexDataBuffer m_vertexDataBuffer;

    std::vector<glm::vec3> m_vertices;

    uint32_t m_verticesCount{ 0 };

    std::vector<uint32_t> m_indices;

    std::vector<prev_test::render::MeshPart> m_meshParts;
};
} // namespace prev_test::render::mesh

#endif // !__MESH_H__
