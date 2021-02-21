#include "SphereMesh.h"

#include "MeshUtil.h"

namespace prev_test::render::mesh {
SphereMesh::SphereMesh(const float radius, const int subDivY, const int subDivZ, const float degreesHorizontal, const float degreesVertical, bool generateTangentBiTangent)
{
    if (generateTangentBiTangent) {
        m_vertexLayout = { { prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC2, prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC3 } };
    } else {
        m_vertexLayout = { { prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC2, prev_test::render::VertexLayoutComponent::VEC3 } };
    }

    std::vector<glm::vec2> textureCoords;
    std::vector<glm::vec3> normals;

    uint32_t indexBase{ 0 };

    const float addAngleY{ -degreesHorizontal / static_cast<float>(subDivY) };
    const float addAngleZ{ degreesVertical / static_cast<float>(subDivZ) };

    float curAngleY = 0.0f;
    int stepsY = 1;
    while (stepsY <= subDivY) {
        const float sinY = sinf(glm::radians(curAngleY));
        const float cosY = cosf(glm::radians(curAngleY));
        const glm::vec3 directionY(cosY, 0.0f, -sinY);

        const float nextAngleY = curAngleY + addAngleY;
        const float nextSinY = sinf(glm::radians(nextAngleY));
        const float nextCosY = cosf(glm::radians(nextAngleY));
        const glm::vec3 nextDirectionY(nextCosY, 0.0f, -nextSinY);

        float currentAngleZ = 0.0f;
        int stepsZ = 1;
        while (stepsZ <= subDivZ) {
            const float sinZ = sinf(glm::radians(currentAngleZ));
            const float cosZ = cosf(glm::radians(currentAngleZ));

            const float nextAngleZ = currentAngleZ + addAngleZ;
            const float nextSinZ = sinf(glm::radians(nextAngleZ));
            const float nextCosZ = cosf(glm::radians(nextAngleZ));

            const glm::vec3 quadPoints[] = {
                { directionY.x * sinZ * radius, cosZ * radius, directionY.z * sinZ * radius },
                { directionY.x * nextSinZ * radius, nextCosZ * radius, directionY.z * nextSinZ * radius },
                { nextDirectionY.x * nextSinZ * radius, nextCosZ * radius, nextDirectionY.z * nextSinZ * radius },
                { nextDirectionY.x * sinZ * radius, cosZ * radius, nextDirectionY.z * sinZ * radius }
            };

            const glm::vec3 ns[] = {
                glm::normalize(quadPoints[0]),
                glm::normalize(quadPoints[1]),
                glm::normalize(quadPoints[2]),
                glm::normalize(quadPoints[3])
            };

            glm::vec2 tcs[4];
            float offset = 0.75f;
            float tx1 = 0.0f;
            float ty1 = 0.0f;
            for (unsigned int i = 0; i < 4; i++) {
                float tx = atan2f(ns[i].x, ns[i].z) / glm::radians(fabsf(degreesHorizontal)) + 0.5f;
                float ty = asinf(ns[i].y) / glm::radians(fabsf(degreesVertical)) + 0.5f;
                if (i > 0) {
                    if (tx < offset && tx1 > offset) {
                        tx += 1.0f;
                    } else if (tx > offset && tx1 < offset) {
                        tx -= 1.0f;
                    }
                } else {
                    tx1 = tx;
                    ty1 = ty;
                }
                tcs[i] = glm::vec2(tx, ty);
            }

            for (auto i = 0; i < 4; i++) {
                m_vertices.push_back(quadPoints[i]);
                textureCoords.push_back(tcs[i]);
                normals.push_back(ns[i]);
            }

            const uint32_t quadIndices[] = { 0, 1, 2, 2, 3, 0 };
            for (const auto idx : quadIndices) {
                m_indices.push_back(indexBase + idx);
            }
            indexBase += 4;

            stepsZ++;
            currentAngleZ += addAngleZ;
        }
        stepsY++;
        curAngleY += addAngleY;
    }

    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> biTangents;
    if (generateTangentBiTangent) {
        prev_test::render::mesh::MeshUtil::GenerateTangetsAndBiTangents(m_vertices, textureCoords, m_indices, tangents, biTangents);
    }

    for (auto vertexIndex = 0; vertexIndex < m_vertices.size(); vertexIndex++) {
        m_vertexDataBuffer.Add(m_vertices[vertexIndex]);
        m_vertexDataBuffer.Add(textureCoords[vertexIndex]);
        m_vertexDataBuffer.Add(normals[vertexIndex]);
        if (generateTangentBiTangent) {
            m_vertexDataBuffer.Add(tangents[vertexIndex]);
            m_vertexDataBuffer.Add(biTangents[vertexIndex]);
        }
    }

    m_meshParts.push_back(prev_test::render::MeshPart(static_cast<uint32_t>(m_indices.size()), m_vertices));
    m_meshRootNode = prev_test::render::MeshNode{ { 0 }, glm::mat4(1.0f), {} };
}

const prev_test::render::VertexLayout& SphereMesh::GetVertexLayout() const
{
    return m_vertexLayout;
}

const void* SphereMesh::GetVertexData() const
{
    return m_vertexDataBuffer.GetData();
}

uint32_t SphereMesh::GerVerticesCount() const
{
    return static_cast<uint32_t>(m_vertices.size());
}

const std::vector<uint32_t>& SphereMesh::GetIndices() const
{
    return m_indices;
}

const std::vector<prev_test::render::MeshPart>& SphereMesh::GetMeshParts() const
{
    return m_meshParts;
}

const MeshNode& SphereMesh::GetRootNode() const
{
    return m_meshRootNode;
}
} // namespace prev_test::render::mesh