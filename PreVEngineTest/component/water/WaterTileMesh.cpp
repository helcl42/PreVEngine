#include "WaterTileMesh.h"

namespace prev_test::component::water {
const prev_test::render::VertexLayout& WaterTileMesh::GetVertexLayout() const
{
    return vertexLayout;
}

const void* WaterTileMesh::GetVertexData() const
{
    return (const void*)vertices.data();
}

const std::vector<glm::vec3>& WaterTileMesh::GetVertices() const
{
    return vertices;
}

uint32_t WaterTileMesh::GerVerticesCount() const
{
    return static_cast<uint32_t>(vertices.size());
}

const std::vector<uint32_t>& WaterTileMesh::GetIndices() const
{
    return indices;
}

const std::vector<prev_test::render::MeshPart>& WaterTileMesh::GetMeshParts() const
{
    return meshParts;
}
} // namespace prev_test::component::water