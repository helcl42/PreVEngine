#include "FullScreenQuadMesh.h"

namespace prev_test::render::mesh {
FullScreenQuadMesh::FullScreenQuadMesh()
{
    for (auto i = 0; i < vertices.size(); i++) {
        m_vertexDataBuffer.Add(vertices[i]);
        m_vertexDataBuffer.Add(textureCoords[i]);
        m_vertexDataBuffer.Add(normals[i]);
    }
}

const prev_test::render::VertexLayout& FullScreenQuadMesh::GetVertexLayout() const
{
    return m_vertexLayout;
}

const void* FullScreenQuadMesh::GetVertexData() const
{
    return m_vertexDataBuffer.GetData();
}

const std::vector<glm::vec3>& FullScreenQuadMesh::GetVertices() const
{
    return vertices;
}

uint32_t FullScreenQuadMesh::GerVerticesCount() const
{
    return static_cast<uint32_t>(vertices.size());
}

const std::vector<uint32_t>& FullScreenQuadMesh::GetIndices() const
{
    return indices;
}

const std::vector<MeshPart>& FullScreenQuadMesh::GetMeshParts() const
{
    return meshParts;
}

const MeshNode& FullScreenQuadMesh::GetRootNode() const
{
    return meshRootNode;
}
} // namespace prev_test::render::mesh