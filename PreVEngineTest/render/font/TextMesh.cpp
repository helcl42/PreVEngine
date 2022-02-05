#include "TextMesh.h"

namespace prev_test::render::font {
TextMesh::TextMesh(const std::vector<glm::vec2>& vertices, const std::vector<glm::vec2>& textureCoords, const std::vector<uint32_t>& indices)
    : m_vertexLayout({ prev_test::render::VertexLayoutComponent::VEC2, prev_test::render::VertexLayoutComponent::VEC2 })
    , m_verticesCount(static_cast<uint32_t>(vertices.size()))
    , m_indices(indices)
{
    for (size_t i = 0; i < vertices.size(); i++) {
        const auto pt2d{ vertices[i] };
        const auto pt3d{ glm::vec3{ pt2d.x, pt2d.y, 0.0f } };
        m_vertexDataBuffer.Add(pt2d);
        m_vertexDataBuffer.Add(textureCoords[i]);
        m_vertices.push_back(pt3d);
    }

    m_meshParts.emplace_back(prev_test::render::MeshPart(static_cast<uint32_t>(m_indices.size()), m_vertices));
}

const prev_test::render::VertexLayout& TextMesh::GetVertexLayout() const
{
    return m_vertexLayout;
}

const void* TextMesh::GetVertexData() const
{
    return m_vertexDataBuffer.GetData();
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

const MeshNode& TextMesh::GetRootNode() const
{
    return m_meshRootNode;
}
} // namespace prev_test::render::font
