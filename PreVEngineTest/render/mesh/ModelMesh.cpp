#include "ModelMesh.h"

namespace prev_test::render::mesh {
const prev_test::render::VertexLayout& ModelMesh::GetVertexLayout() const
{
    return m_vertexLayout;
}

const void* ModelMesh::GetVertexData() const
{
    return m_vertexDataBuffer.GetData();
}

const std::vector<glm::vec3>& ModelMesh::GetVertices() const
{
    return m_vertices;
}

uint32_t ModelMesh::GerVerticesCount() const
{
    return m_verticesCount;
}

const std::vector<uint32_t>& ModelMesh::GetIndices() const
{
    return m_indices;
}

const std::vector<prev_test::render::MeshPart>& ModelMesh::GetMeshParts() const
{
    return m_meshParts;
}

} // namespace prev_test::render::mesh