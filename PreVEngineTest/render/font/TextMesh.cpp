#include "TextMesh.h"

namespace prev_test::render::font {
TextMesh::TextMesh(const std::vector<glm::vec2>& vertices, const std::vector<glm::vec2>& textureCoords, const std::vector<uint32_t>& indices)
    : m_vertexLayout({ prev_test::render::VertexLayoutComponent::VEC2, prev_test::render::VertexLayoutComponent::VEC2 })
    , m_verticesCount(static_cast<uint32_t>(vertices.size()))
    , m_indices(indices)
{
    for (size_t i = 0; i < vertices.size(); i++) {
        m_vertexDataBuffer.Add(vertices[i]);
        m_vertexDataBuffer.Add(textureCoords[i]);
        m_vertices.push_back(glm::vec3(vertices[i].x, vertices[i].y, 0.0f));
    }

    m_meshParts.push_back(prev_test::render::MeshPart(static_cast<uint32_t>(m_indices.size())));
}

const prev_test::render::VertexLayout& TextMesh::GetVertexLayout() const
{
    return m_vertexLayout;
}

const void* TextMesh::GetVertexData() const
{
    return m_vertexDataBuffer.GetData();
}

const std::vector<glm::vec3>& TextMesh::GetVertices() const
{
    return m_vertices;
}

uint32_t TextMesh::GerVerticesCount() const
{
    return m_verticesCount;
}

const std::vector<uint32_t>& TextMesh::GetIndices() const
{
    return m_indices;
}

const std::vector<prev_test::render::MeshPart>& TextMesh::GetMeshParts() const
{
    return m_meshParts;
}
} // namespace prev_test::render::font
