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

uint32_t ModelMesh::GerVerticesCount() const
{
    return m_verticesCount;
}

const std::vector<uint32_t>& ModelMesh::GetIndices() const
{
    return m_indices;
}

uint32_t ModelMesh::GetIndicesCount() const
{
    return static_cast<uint32_t>(m_indices.size());
}

const std::vector<prev_test::render::MeshPart>& ModelMesh::GetMeshParts() const
{
    return m_meshParts;
}

const MeshNode& ModelMesh::GetRootNode() const
{
    return m_meshRootNode;
}
} // namespace prev_test::render::mesh