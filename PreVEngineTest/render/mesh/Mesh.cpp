#include "Mesh.h"

namespace prev_test::render::mesh {
const prev_test::render::VertexLayout& Mesh::GetVertexLayout() const
{
    return m_vertexLayout;
}

const void* Mesh::GetVertexData() const
{
    return m_vertexDataBuffer.GetData();
}

const std::vector<glm::vec3>& Mesh::GetVertices() const
{
    return m_vertices;
}

uint32_t Mesh::GerVerticesCount() const
{
    return m_verticesCount;
}

const std::vector<uint32_t>& Mesh::GetIndices() const
{
    return m_indices;
}

const std::vector<prev_test::render::MeshPart>& Mesh::GetMeshParts() const
{
    return m_meshParts;
}

}