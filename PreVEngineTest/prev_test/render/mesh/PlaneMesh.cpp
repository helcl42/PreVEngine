#include "PlaneMesh.h"

#include "MeshUtil.h"

namespace prev_test::render::mesh {
namespace {
    glm::vec3 CreateConstellatedVertex(const FlatMeshConstellation constellation, const float x, const float z)
    {
        switch (constellation) {
        case FlatMeshConstellation::ZERO_X:
            return { 0.0f, x, z };
        case FlatMeshConstellation::ZERO_Z:
            return { x, z, 0.0f };
        default:
            return { x, 0.0f, z };
        }
    }

    glm::vec3 CreateConstellatedNormal(const FlatMeshConstellation constellation)
    {
        switch (constellation) {
        case FlatMeshConstellation::ZERO_X:
            return { 1.0f, 0.0f, 0.0f };
        case FlatMeshConstellation::ZERO_Z:
            return { 0.0f, 0.0f, 1.0f };
        default:
            return { 0.0f, 1.0f, 0.0f };
        }
    }
} // namespace

PlaneMesh::PlaneMesh(const float xSize, const float zSize, const uint32_t xDivs, const uint32_t zDivs, const float textureCoordUMax, const float textureCoordVMax, const FlatMeshConstellation constellation, const bool generateTangentBiTangent)
{
    if (generateTangentBiTangent) {
        m_vertexLayout = { { prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC2, prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC3 } };
    } else {
        m_vertexLayout = { { prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC2, prev_test::render::VertexLayoutComponent::VEC3 } };
    }

    const float x2{ xSize / 2.0f };
    const float z2{ zSize / 2.0f };
    const float iFactor{ zSize / static_cast<float>(zDivs) };
    const float jFactor{ xSize / static_cast<float>(xDivs) };
    const float texi{ textureCoordUMax / static_cast<float>(zDivs) };
    const float texj{ textureCoordVMax / static_cast<float>(xDivs) };

    std::vector<glm::vec2> textureCoords;
    std::vector<glm::vec3> normals;

    for (uint32_t i = 0; i <= zDivs; ++i) {
        const float z{ iFactor * i - z2 };
        for (uint32_t j = 0; j <= xDivs; ++j) {
            const float x{ jFactor * j - x2 };

            const glm::vec3 vertex{ CreateConstellatedVertex(constellation, x, z) };
            const glm::vec2 textureCoord{ j * texj, i * texi };
            const glm::vec3 normal{ CreateConstellatedNormal(constellation) };

            m_vertices.push_back(vertex);
            textureCoords.push_back(textureCoord);
            normals.push_back(normal);
        }
    }

    for (uint32_t i = 0; i < zDivs; ++i) {
        const uint32_t rowStart{ i * (xDivs + 1) };
        const uint32_t nextRowStart{ (i + 1) * (xDivs + 1) };
        for (uint32_t j = 0; j < xDivs; j++) {
            const uint32_t indices[] = {
                rowStart + j,
                rowStart + j + 1,
                nextRowStart + j + 1,

                nextRowStart + j + 1,
                nextRowStart + j,
                rowStart + j,
            };

            for (const auto index : indices) {
                m_indices.emplace_back(index);
            }
        }
    }

    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> biTangents;
    if (generateTangentBiTangent) {
        std::tie(tangents, biTangents) = prev_test::render::mesh::MeshUtil::GenerateTangetsAndBiTangents(m_vertices, textureCoords, normals, m_indices);
    }

    for (size_t vertexIndex = 0; vertexIndex < m_vertices.size(); vertexIndex++) {
        m_vertexDataBuffer.Add(m_vertices[vertexIndex]);
        m_vertexDataBuffer.Add(textureCoords[vertexIndex]);
        m_vertexDataBuffer.Add(normals[vertexIndex]);
        if (generateTangentBiTangent) {
            m_vertexDataBuffer.Add(tangents[vertexIndex]);
            m_vertexDataBuffer.Add(biTangents[vertexIndex]);
        }
    }

    m_meshParts.emplace_back(prev_test::render::MeshPart(static_cast<uint32_t>(m_indices.size()), m_vertices));
    m_meshRootNode = prev_test::render::MeshNode{ { 0 }, glm::mat4(1.0f), {} };
}

const prev_test::render::VertexLayout& PlaneMesh::GetVertexLayout() const
{
    return m_vertexLayout;
}

const void* PlaneMesh::GetVertexData() const
{
    return m_vertexDataBuffer.GetData();
}

uint32_t PlaneMesh::GerVerticesCount() const
{
    return static_cast<uint32_t>(m_vertices.size());
}

const std::vector<uint32_t>& PlaneMesh::GetIndices() const
{
    return m_indices;
}

const std::vector<prev_test::render::MeshPart>& PlaneMesh::GetMeshParts() const
{
    return m_meshParts;
}

const MeshNode& PlaneMesh::GetRootNode() const
{
    return m_meshRootNode;
}
} // namespace prev_test::render::mesh