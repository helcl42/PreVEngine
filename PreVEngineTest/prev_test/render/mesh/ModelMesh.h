#ifndef __MODEL_MESH_H__
#define __MODEL_MESH_H__

#include "../IMesh.h"
#include "../VertexDataBuffer.h"

namespace prev_test::render::mesh {
class ModelMeshFactory;

class ModelMesh final : public prev_test::render::IMesh {
public:
    const prev_test::render::VertexLayout& GetVertexLayout() const override;

    const void* GetVertexData() const override;

    uint32_t GerVerticesCount() const override;

    const std::vector<uint32_t>& GetIndices() const override;

    const std::vector<prev_test::render::MeshPart>& GetMeshParts() const override;

    const MeshNode& GetRootNode() const override;

private:
    friend ModelMeshFactory;

private:
    prev_test::render::VertexLayout m_vertexLayout;

    prev_test::render::VertexDataBuffer m_vertexDataBuffer;

    std::vector<glm::vec3> m_vertices;

    uint32_t m_verticesCount{ 0 };

    std::vector<uint32_t> m_indices;

    std::vector<prev_test::render::MeshPart> m_meshParts;

    prev_test::render::MeshNode m_meshRootNode;
};
} // namespace prev_test::render::mesh

#endif // !__MODEL_MESH_H__
