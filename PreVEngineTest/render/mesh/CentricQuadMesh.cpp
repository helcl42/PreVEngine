#include "CentricQuadMesh.h"

namespace prev_test::render::mesh {
const prev_test::render::VertexLayout& CentricQuadMesh::GetVertexLayout() const
{
    return vertexLayout;
}

const void* CentricQuadMesh::GetVertexData() const
{
    return (const void*)vertices.data();
}

const std::vector<glm::vec3>& CentricQuadMesh::GetVertices() const
{
    return vertices;
}

uint32_t CentricQuadMesh::GerVerticesCount() const
{
    return static_cast<uint32_t>(vertices.size());
}

const std::vector<uint32_t>& CentricQuadMesh::GetIndices() const
{
    return indices;
}

const std::vector<prev_test::render::MeshPart>& CentricQuadMesh::GetMeshParts() const
{
    return meshParts;
}

const MeshNode& CentricQuadMesh::GetRootNode() const
{
    return meshRootNode;
}
} // namespace prev_test::render::mesh