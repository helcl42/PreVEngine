#include "TerrainMesh.h"

namespace prev_test::component::terrain {
const prev_test::render::VertexLayout& TerrainMesh::GetVertexLayout() const
{
    return m_vertexLayout;
}

const void* TerrainMesh::GetVertexData() const
{
    return m_vertexDataBuffer.GetData();
}

const std::vector<glm::vec3>& TerrainMesh::GetVertices() const
{
    return m_vertices;
}

uint32_t TerrainMesh::GerVerticesCount() const
{
    return m_verticesCount;
}

const std::vector<uint32_t>& TerrainMesh::GetIndices() const
{
    return m_indices;
}

const std::vector<prev_test::render::MeshPart>& TerrainMesh::GetMeshParts() const
{
    return m_meshParts;
}
} // namespace prev_test::component::terrain