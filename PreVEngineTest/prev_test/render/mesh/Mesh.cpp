#include "Mesh.h"

namespace prev_test::render::mesh {
Mesh::Mesh(const prev_test::render::VertexLayout& vertexLayout, const prev_test::render::VertexDataBuffer& vertexDataBuffer, const std::vector<uint32_t>& indices, const prev_test::render::MeshNode& meshRootNode, const std::vector<prev_test::render::MeshPart>& meshParts)
    : m_vertexLayout{ vertexLayout }
    , m_vertexDataBuffer{ vertexDataBuffer }
    , m_indices{ indices }
    , m_meshRootNode{ meshRootNode }
    , m_meshParts{ meshParts }
{
}

const prev_test::render::VertexLayout& Mesh::GetVertexLayout() const
{
    return m_vertexLayout;
}

const void* Mesh::GetVertexData() const
{
    return m_vertexDataBuffer.GetData();
}

uint32_t Mesh::GerVerticesCount() const
{
    return m_vertexDataBuffer.GetSize() / m_vertexLayout.GetStride();
}

const std::vector<uint32_t>& Mesh::GetIndices() const
{
    return m_indices;
}

uint32_t Mesh::GetIndicesCount() const
{
    return static_cast<uint32_t>(m_indices.size());
}

const std::vector<prev_test::render::MeshPart>& Mesh::GetMeshParts() const
{
    return m_meshParts;
}

const MeshNode& Mesh::GetRootNode() const
{
    return m_meshRootNode;
}
} // namespace prev_test::render::mesh
