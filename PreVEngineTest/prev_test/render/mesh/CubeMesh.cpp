#include "CubeMesh.h"
#include "MeshUtil.h"

namespace prev_test::render::mesh {
CubeMesh::CubeMesh(const bool includeTangentBiTangent)
{
    auto normals{ prev_test::render::mesh::MeshUtil::GenerateNormals(vertices, indices, false) };

    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> biTangents;
    if (includeTangentBiTangent) {
        std::tie(tangents, biTangents) = prev_test::render::mesh::MeshUtil::GenerateTangetsAndBiTangents(vertices, textureCoords, normals, indices);
        m_vertexLayout = { { prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC2, prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC3 } };
    } else {
        m_vertexLayout = { { prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC2, prev_test::render::VertexLayoutComponent::VEC3 } };
    }

    for (size_t i = 0; i < vertices.size(); i++) {
        m_vertexDataBuffer.Add(vertices[i]);
        m_vertexDataBuffer.Add(textureCoords[i]);
        m_vertexDataBuffer.Add(normals[i]);
        if (includeTangentBiTangent) {
            m_vertexDataBuffer.Add(tangents[i]);
            m_vertexDataBuffer.Add(biTangents[i]);
        }
    }
}

const prev_test::render::VertexLayout& CubeMesh::GetVertexLayout() const
{
    return m_vertexLayout;
}

const void* CubeMesh::GetVertexData() const
{
    return m_vertexDataBuffer.GetData();
}

uint32_t CubeMesh::GerVerticesCount() const
{
    return static_cast<uint32_t>(vertices.size());
}

const std::vector<uint32_t>& CubeMesh::GetIndices() const
{
    return indices;
}

uint32_t CubeMesh::GetIndicesCount() const
{
    return static_cast<uint32_t>(indices.size());
}

const std::vector<prev_test::render::MeshPart>& CubeMesh::GetMeshParts() const
{
    return meshParts;
}

const MeshNode& CubeMesh::GetRootNode() const
{
    return meshRootNode;
}
} // namespace prev_test::render::mesh