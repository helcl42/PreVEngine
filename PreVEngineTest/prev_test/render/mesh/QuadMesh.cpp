#include "QuadMesh.h"

#include "MeshUtil.h"

namespace prev_test::render::mesh {
namespace {
    std::vector<glm::vec3> CreateConstalatedVertices(const FlatMeshConstellation constellation)
    {
        std::vector<glm::vec3> vertices = {
            { 1.0f, 1.0f, 0.0f },
            { -1.0f, 1.0f, 0.0f },
            { -1.0f, -1.0f, 0.0f },
            { 1.0f, -1.0f, 0.0f }
        };

        if (constellation == FlatMeshConstellation::ZERO_X) {
            for (auto& v : vertices) {
                v = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(v, 1.0f);
            }
        } else if (constellation == FlatMeshConstellation::ZERO_Y) {
            for (auto& v : vertices) {
                v = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::vec4(v, 1.0f);
            }
        }
        return vertices;
    }
} // namespace

QuadMesh::QuadMesh(const FlatMeshConstellation constellation, const bool includeTangentBiTangent)
{
    const std::vector<glm::vec3> vertices{ CreateConstalatedVertices(constellation) };

    const std::vector<glm::vec2> textureCoords = {
        { 1.0f, 1.0f },
        { 0.0f, 1.0f },
        { 0.0f, 0.0f },
        { 1.0f, 0.0f }
    };

    const std::vector<uint32_t> indices = {
        0, 2, 1, 2, 0, 3
    };

    const auto normals{ prev_test::render::mesh::MeshUtil::GenerateNormals(vertices, indices, false) };

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

    m_vertices = vertices;
    m_indices = indices;

    m_meshParts.push_back(MeshPart(static_cast<uint32_t>(indices.size()), vertices));
    m_meshRootNode = prev_test::render::MeshNode{ { 0 }, glm::mat4(1.0f), {} };
}

const prev_test::render::VertexLayout& QuadMesh::GetVertexLayout() const
{
    return m_vertexLayout;
}

const void* QuadMesh::GetVertexData() const
{
    return m_vertexDataBuffer.GetData();
}

uint32_t QuadMesh::GerVerticesCount() const
{
    return static_cast<uint32_t>(m_vertices.size());
}

const std::vector<uint32_t>& QuadMesh::GetIndices() const
{
    return m_indices;
}

const std::vector<MeshPart>& QuadMesh::GetMeshParts() const
{
    return m_meshParts;
}

const MeshNode& QuadMesh::GetRootNode() const
{
    return m_meshRootNode;
}
} // namespace prev_test::render::mesh