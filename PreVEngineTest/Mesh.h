#ifndef __MESH_H__
#define __MESH_H__

#include <Utils.h>

#include "AssimpCommon.h"

class CubeMesh final : public IMesh {
public:
    CubeMesh(const bool includeTangentBiTangent = false)
    {
        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> biTangents;
        if (includeTangentBiTangent) {
            MeshUtil::GenerateTangetsAndBiTangents(vertices, textureCoords, indices, tangents, biTangents);
            m_vertexLayout = { { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 } };
        } else {
            m_vertexLayout = { { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 } };
        }

        for (auto i = 0; i < vertices.size(); i++) {
            m_vertexDataBuffer.Add(vertices[i]);
            m_vertexDataBuffer.Add(textureCoords[i]);
            m_vertexDataBuffer.Add(normals[i]);
            if (includeTangentBiTangent) {
                m_vertexDataBuffer.Add(tangents[i]);
                m_vertexDataBuffer.Add(biTangents[i]);
            }
        }
    }

    ~CubeMesh() = default;
    
public:
    const VertexLayout& GetVertexLayout() const override
    {
        return m_vertexLayout;
    }

    const void* GetVertexData() const override
    {
        return m_vertexDataBuffer.GetData();
    }

    const std::vector<glm::vec3>& GetVertices() const override
    {
        return vertices;
    }

    uint32_t GerVerticesCount() const override
    {
        return static_cast<uint32_t>(vertices.size());
    }

    const std::vector<uint32_t>& GetIndices() const override
    {
        return indices;
    }

    const std::vector<MeshPart>& GetMeshParts() const override
    {
        return meshParts;
    }

private:
    VertexLayout m_vertexLayout;
   
    VertexDataBuffer m_vertexDataBuffer;

private:
    static const inline std::vector<glm::vec3> vertices{
        // FROMT
        { -0.5f, -0.5f, 0.5f },
        { 0.5f, -0.5f, 0.5f },
        { 0.5f, 0.5f, 0.5f },
        { -0.5f, 0.5f, 0.5f },
        // BACK
        { -0.5f, -0.5f, -0.5f },
        { 0.5f, -0.5f, -0.5f },
        { 0.5f, 0.5f, -0.5f },
        { -0.5f, 0.5f, -0.5f },
        // TOP
        { -0.5f, 0.5f, 0.5f },
        { 0.5f, 0.5f, 0.5f },
        { 0.5f, 0.5f, -0.5f },
        { -0.5f, 0.5f, -0.5f },
        // BOTTOM
        { -0.5f, -0.5f, 0.5f },
        { 0.5f, -0.5f, 0.5f },
        { 0.5f, -0.5f, -0.5f },
        { -0.5f, -0.5f, -0.5f },
        // LEFT
        { -0.5f, -0.5f, 0.5f },
        { -0.5f, 0.5f, 0.5f },
        { -0.5f, 0.5f, -0.5f },
        { -0.5f, -0.5f, -0.5f },
        // RIGHT
        { 0.5f, -0.5f, 0.5f },
        { 0.5f, 0.5f, 0.5f },
        { 0.5f, 0.5f, -0.5f },
        { 0.5f, -0.5f, -0.5f }
    };

    static const inline std::vector<glm::vec2> textureCoords{
        // FROMT
        { 1.0f, 0.0f },
        { 0.0f, 0.0f },
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        // BACK
        { 1.0f, 0.0f },
        { 0.0f, 0.0f },
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        // TOP
        { 1.0f, 0.0f },
        { 0.0f, 0.0f },
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        // BOTTOM
        { 1.0f, 0.0f },
        { 0.0f, 0.0f },
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        // LEFT
        { 1.0f, 0.0f },
        { 0.0f, 0.0f },
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        // RIGHT
        { 1.0f, 0.0f },
        { 0.0f, 0.0f },
        { 0.0f, 1.0f },
        { 1.0f, 1.0f }
    };

    static const inline std::vector<glm::vec3> normals{
        // FROMT
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
        // BACK
        { 0.0f, 0.0f, -1.0f },
        { 0.0f, 0.0f, -1.0f },
        { 0.0f, 0.0f, -1.0f },
        { 0.0f, 0.0f, -1.0f },
        // TOP
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        // BOTTOM
        { 0.0f, -1.0f, 0.0f },
        { 0.0f, -1.0f, 0.0f },
        { 0.0f, -1.0f, 0.0f },
        { 0.0f, -1.0f, 0.0f },
        // LEFT
        { -1.0f, 0.0f, 0.0f },
        { -1.0f, 0.0f, 0.0f },
        { -1.0f, 0.0f, 0.0f },
        { -1.0f, 0.0f, 0.0f },
        // RIGHT
        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
    };

    static const inline std::vector<uint32_t> indices{
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };

    static const inline std::vector<MeshPart> meshParts = {
        MeshPart(static_cast<uint32_t>(indices.size()))
    };
};

class QuadMesh final : public IMesh {
public:
    QuadMesh(const bool includeTangentBiTangent = false) {
        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> biTangents;
        if (includeTangentBiTangent) {
            MeshUtil::GenerateTangetsAndBiTangents(vertices, textureCoords, indices, tangents, biTangents);
            m_vertexLayout = { { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 } };
        } else {
            m_vertexLayout = { { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 } };
        }

        for (auto i = 0; i < vertices.size(); i++) {
            m_vertexDataBuffer.Add(vertices[i]);
            m_vertexDataBuffer.Add(textureCoords[i]);
            m_vertexDataBuffer.Add(normals[i]);
            if (includeTangentBiTangent) {
                m_vertexDataBuffer.Add(tangents[i]);
                m_vertexDataBuffer.Add(biTangents[i]);
            }
        }
    }

    ~QuadMesh() = default;

public:
    const VertexLayout& GetVertexLayout() const override
    {
        return m_vertexLayout;
    }

    const void* GetVertexData() const override
    {
        return m_vertexDataBuffer.GetData();
    }

    const std::vector<glm::vec3>& GetVertices() const override
    {
        return vertices;
    }

    uint32_t GerVerticesCount() const override
    {
        return static_cast<uint32_t>(vertices.size());
    }

    const std::vector<uint32_t>& GetIndices() const override
    {
        return indices;
    }

    const std::vector<MeshPart>& GetMeshParts() const override
    {
        return meshParts;
    }

private:
    VertexLayout m_vertexLayout;

    VertexDataBuffer m_vertexDataBuffer;

private:
    static const inline std::vector<glm::vec3> vertices{
        { 1.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f }
    };

    static const inline std::vector<glm::vec2> textureCoords{
        { 1.0f, 1.0f },
        { 0.0f, 1.0f },
        { 0.0f, 0.0f },
        { 1.0f, 0.0f }
    };

    static const inline std::vector<glm::vec3> normals{
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f }
    };

    static const inline std::vector<uint32_t> indices{
        0, 1, 2, 2, 3, 0
    };

    static const inline std::vector<MeshPart> meshParts = {
        MeshPart(static_cast<uint32_t>(indices.size()))
    };
};

class FullScreenQuadMesh final : public IMesh {
public:
    FullScreenQuadMesh()
    {
        for (auto i = 0; i < vertices.size(); i++) {
            m_vertexDataBuffer.Add(vertices[i]);
            m_vertexDataBuffer.Add(textureCoords[i]);
            m_vertexDataBuffer.Add(normals[i]);
        }
    }

    ~FullScreenQuadMesh() = default;

public:
    const VertexLayout& GetVertexLayout() const override
    {
        return m_vertexLayout;
    }

    const void* GetVertexData() const override
    {
        return m_vertexDataBuffer.GetData();
    }

    const std::vector<glm::vec3>& GetVertices() const override
    {
        return vertices;
    }

    uint32_t GerVerticesCount() const override
    {
        return static_cast<uint32_t>(vertices.size());
    }

    const std::vector<uint32_t>& GetIndices() const override
    {
        return indices;
    }

    const std::vector<MeshPart>& GetMeshParts() const override
    {
        return meshParts;
    }

private:
    VertexDataBuffer m_vertexDataBuffer;

private:
    static const inline VertexLayout m_vertexLayout{ { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 } };

    static const inline std::vector<glm::vec3> vertices{
        { 1.0f, 1.0f, 0.0f },
        { -1.0f, 1.0f, 0.0f },
        { -1.0f, -1.0f, 0.0f },
        { 1.0f, -1.0f, 0.0f }
    };

    static const inline std::vector<glm::vec2> textureCoords{
        { 1.0f, 1.0f },
        { 0.0f, 1.0f },
        { 0.0f, 0.0f },
        { 1.0f, 0.0f }
    };

    static const inline std::vector<glm::vec3> normals{
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f }
    };

    static const inline std::vector<uint32_t> indices{
        0, 1, 2, 2, 3, 0
    };

    static const inline std::vector<MeshPart> meshParts = {
        MeshPart(static_cast<uint32_t>(indices.size()))
    };
};

class PlaneMesh final : public IMesh {
public:
    PlaneMesh(const float xSize, const float zSize, const uint32_t xDivs, const uint32_t zDivs, const float textureCoordUMax = 1.0f, const float textureCoordVMax = 1.0f, bool generateTangentBiTangent = false)
    {
        if (generateTangentBiTangent) {
            m_vertexLayout = { { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 } };
        } else {
            m_vertexLayout = { { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 } };
        }

        const float x2 = xSize / 2.0f;
        const float z2 = zSize / 2.0f;
        const float iFactor = zSize / static_cast<float>(zDivs);
        const float jFactor = xSize / static_cast<float>(xDivs);
        const float texi = textureCoordUMax / static_cast<float>(zDivs);
        const float texj = textureCoordVMax / static_cast<float>(xDivs);

        std::vector<glm::vec2> textureCoords;
        std::vector<glm::vec3> normals;

        for (uint32_t i = 0; i <= zDivs; i++) {
            float z = iFactor * i - z2;

            for (uint32_t j = 0; j <= xDivs; j++) {
                float x = jFactor * j - x2;

                glm::vec3 vertex(x, 0.0f, z);
                glm::vec2 textureCoord(j * texi, i * texj);
                glm::vec3 normal(0.0f, 1.0f, 0.0f);

                m_vertices.push_back(vertex);
                textureCoords.push_back(textureCoord);
                normals.push_back(normal);
            }
        }

        for (uint32_t i = 0; i < zDivs; i++) {
            const uint32_t rowStart = i * (xDivs + 1);
            const uint32_t nextRowStart = (i + 1) * (xDivs + 1);

            for (uint32_t j = 0; j < xDivs; j++) {
                const uint32_t indices[] = {
                    rowStart + j,
                    nextRowStart + j + 1,
                    nextRowStart + j,
                    rowStart + j,
                    rowStart + j + 1,
                    nextRowStart + j + 1
                };

                for (const auto index : indices) {
                    m_indices.emplace_back(index);
                }
            }
        }

        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> biTangents;
        if (generateTangentBiTangent) {
            MeshUtil::GenerateTangetsAndBiTangents(m_vertices, textureCoords, m_indices, tangents, biTangents);
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

        m_meshParts.push_back(MeshPart(static_cast<uint32_t>(m_indices.size())));
    }

    ~PlaneMesh() = default;

public:
    const VertexLayout& GetVertexLayout() const override
    {
        return m_vertexLayout;
    }

    const void* GetVertexData() const override
    {
        return m_vertexDataBuffer.GetData();
    }

    const std::vector<glm::vec3>& GetVertices() const override
    {
        return m_vertices;
    }

    uint32_t GerVerticesCount() const override
    {
        return static_cast<uint32_t>(m_vertices.size());
    }

    const std::vector<uint32_t>& GetIndices() const override
    {
        return m_indices;
    }

    const std::vector<MeshPart>& GetMeshParts() const override
    {
        return m_meshParts;
    }

private:
    VertexLayout m_vertexLayout;

    VertexDataBuffer m_vertexDataBuffer;

    std::vector<glm::vec3> m_vertices;

    std::vector<uint32_t> m_indices;

    std::vector<MeshPart> m_meshParts;
};

class SphereMesh final : public IMesh {
public:
    SphereMesh(const float radius, const int subDivY, const int subDivZ, const float degreesHorizontal = 360.0f, const float degreesVertical = 180.0f, bool generateTangentBiTangent = false)
    {
        if (generateTangentBiTangent) {
            m_vertexLayout = { { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 } };
        } else {
            m_vertexLayout = { { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 } };
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
            MeshUtil::GenerateTangetsAndBiTangents(m_vertices, textureCoords, m_indices, tangents, biTangents);
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

        m_meshParts.push_back(MeshPart(static_cast<uint32_t>(m_indices.size())));
    }

    ~SphereMesh() = default;

public:
    const VertexLayout& GetVertexLayout() const override
    {
        return m_vertexLayout;
    }

    const void* GetVertexData() const override
    {
        return m_vertexDataBuffer.GetData();
    }

    const std::vector<glm::vec3>& GetVertices() const override
    {
        return m_vertices;
    }

    uint32_t GerVerticesCount() const override
    {
        return static_cast<uint32_t>(m_vertices.size());
    }

    const std::vector<uint32_t>& GetIndices() const override
    {
        return m_indices;
    }

    const std::vector<MeshPart>& GetMeshParts() const override
    {
        return m_meshParts;
    }

private:
    VertexLayout m_vertexLayout;

    VertexDataBuffer m_vertexDataBuffer;

    std::vector<glm::vec3> m_vertices;

    std::vector<uint32_t> m_indices;

    std::vector<MeshPart> m_meshParts;
};

class MeshFactory;

class ModelMesh final : public IMesh {
public:
    const VertexLayout& GetVertexLayout() const override
    {
        return m_vertexLayout;
    }

    const void* GetVertexData() const override
    {
        return m_vertexDataBuffer.GetData();
    }

    const std::vector<glm::vec3>& GetVertices() const override
    {
        return m_vertices;
    }

    uint32_t GerVerticesCount() const override
    {
        return m_verticesCount;
    }

    const std::vector<uint32_t>& GetIndices() const override
    {
        return m_indices;
    }

    const std::vector<MeshPart>& GetMeshParts() const override
    {
        return m_meshParts;
    }

private:
    friend MeshFactory;

private:
    VertexLayout m_vertexLayout;

    VertexDataBuffer m_vertexDataBuffer;

    std::vector<glm::vec3> m_vertices;

    uint32_t m_verticesCount = 0;

    std::vector<uint32_t> m_indices;

    std::vector<MeshPart> m_meshParts;
};

class MeshFactory final {
public:
    enum class AssimpMeshFactoryCreateFlags {
        ANIMATION,
        TANGENT_BITANGENT,
        _
    };

public:
    std::unique_ptr<IMesh> CreateMesh(const std::string& modelPath, const FlagSet<AssimpMeshFactoryCreateFlags>& flags = FlagSet<MeshFactory::AssimpMeshFactoryCreateFlags>{}) const
    {
        Assimp::Importer importer{};
        const aiScene* scene;

        AssimpSceneLoader assimpSceneLoader{};
        if (!assimpSceneLoader.LoadScene(modelPath, &importer, &scene)) {
            throw std::runtime_error("Could not load model: " + modelPath);
        }

        auto mesh = std::make_unique<ModelMesh>();

        mesh->m_vertexLayout = GetVertexLayout(flags);
        mesh->m_verticesCount = ReadMeshes(*scene, flags, mesh->m_vertexDataBuffer, mesh->m_vertices, mesh->m_indices, mesh->m_meshParts);

        return mesh;
    }

private:
    void ReadNodeHierarchy(const aiNode* node, const glm::mat4& parentTransform, std::vector<glm::mat4>& transforms) const
    {
        const auto currentTransform = parentTransform * AssimpGlmConvertor::ToGlmMat4(node->mTransformation);
        if (node->mNumMeshes > 0) {
            transforms.push_back(currentTransform);
        }

        for (uint32_t i = 0; i < node->mNumChildren; i++) {
            ReadNodeHierarchy(node->mChildren[i], currentTransform, transforms);
        }
    }

    VertexLayout GetVertexLayout(const FlagSet<AssimpMeshFactoryCreateFlags>& flags) const
    {
        if (flags & AssimpMeshFactoryCreateFlags::ANIMATION && flags & AssimpMeshFactoryCreateFlags::TANGENT_BITANGENT) {
            return { { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 } };
        } else if (flags & AssimpMeshFactoryCreateFlags::ANIMATION) {
            return { { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4 } };
        } else if (flags & AssimpMeshFactoryCreateFlags::TANGENT_BITANGENT) {
            return { { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 } };
        } else {
            return { { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 } };
        }
    }

    void ReadIndices(const aiMesh& mesh, std::vector<uint32_t>& inOutIndices) const
    {
        for (unsigned int i = 0; i < mesh.mNumFaces; i++) {
            const aiFace& face = mesh.mFaces[i];
            for (unsigned int k = 0; k < face.mNumIndices; k++) {
                inOutIndices.emplace_back(face.mIndices[k]);
            }
        }
    }

    void AddDefaultVertexData(const aiMesh& mesh, const uint32_t vertexIndex, VertexDataBuffer& inOutVertexBuffer, std::vector<glm::vec3>& inOutVertices) const
    {
        glm::vec3 pos = glm::make_vec3(&mesh.mVertices[vertexIndex].x);
        glm::vec2 uv = mesh.mTextureCoords[0] != nullptr ? glm::make_vec3(&mesh.mTextureCoords[0][vertexIndex].x) : glm::vec2(1.0f, 1.0f);
        glm::vec3 normal = mesh.HasNormals() ? glm::make_vec3(&mesh.mNormals[vertexIndex].x) : glm::vec3(0.0f, 1.0f, 0.0f);

        inOutVertices.push_back(pos);

        inOutVertexBuffer.Add(pos);
        inOutVertexBuffer.Add(uv);
        inOutVertexBuffer.Add(normal);
    }

    void AddAnimationData(const std::vector<VertexBoneData>& vertexBoneData, const uint32_t vertexIndex, VertexDataBuffer& inOutVertexBuffer) const
    {
        const auto& singleVertexBoneData = vertexBoneData[vertexIndex];

        inOutVertexBuffer.Add(&singleVertexBoneData.ids, static_cast<uint32_t>(ArraySize(singleVertexBoneData.ids) * sizeof(unsigned int)));
        inOutVertexBuffer.Add(&singleVertexBoneData.weights, static_cast<uint32_t>(ArraySize(singleVertexBoneData.weights) * sizeof(float)));
    }

    void AddBumpMappingData(const aiMesh& mesh, const unsigned int vertexIndex, VertexDataBuffer& inOutVertexBuffer) const
    {
        glm::vec3 tangent = glm::make_vec3(&mesh.mTangents[vertexIndex].x);
        glm::vec3 biTangent = glm::make_vec3(&mesh.mBitangents[vertexIndex].x);
        inOutVertexBuffer.Add(tangent);
        inOutVertexBuffer.Add(biTangent);
    }

    void ReadVertexData(const aiMesh& mesh, const FlagSet<AssimpMeshFactoryCreateFlags>& flags, const std::vector<VertexBoneData>& vertexBoneData, const uint32_t vertexBaseOffset, VertexDataBuffer& inOutVertexBuffer, std::vector<glm::vec3>& inOutVertices) const
    {
        for (unsigned int vertexIndex = 0; vertexIndex < mesh.mNumVertices; vertexIndex++) {
            AddDefaultVertexData(mesh, vertexIndex, inOutVertexBuffer, inOutVertices);

            if (flags & AssimpMeshFactoryCreateFlags::ANIMATION) {
                AddAnimationData(vertexBoneData, vertexBaseOffset + vertexIndex, inOutVertexBuffer);
            }

            if (flags & AssimpMeshFactoryCreateFlags::TANGENT_BITANGENT) {
                AddBumpMappingData(mesh, vertexIndex, inOutVertexBuffer);
            }
        }
    }

    std::vector<VertexBoneData> LoadAnimationBones(const aiMesh& mesh, const uint32_t vertexBaseOffset) const
    {
        std::vector<VertexBoneData> bones(mesh.mNumVertices);
        std::map<std::string, uint32_t> boneMapping;

        for (uint32_t boneIndex = 0; boneIndex < mesh.mNumBones; boneIndex++) {
            const std::string currentBoneName{ mesh.mBones[boneIndex]->mName.data };

            uint32_t currentBoneIndex{ 0 };
            if (boneMapping.find(currentBoneName) == boneMapping.end()) {
                currentBoneIndex = static_cast<uint32_t>(boneMapping.size());
                boneMapping[currentBoneName] = currentBoneIndex;
            } else {
                currentBoneIndex = boneMapping[currentBoneName];
            }

            for (uint32_t j = 0; j < mesh.mBones[boneIndex]->mNumWeights; j++) {
                const uint32_t vertexId = vertexBaseOffset + mesh.mBones[boneIndex]->mWeights[j].mVertexId;
                const float weight = mesh.mBones[boneIndex]->mWeights[j].mWeight;
                bones[vertexId].AddBoneData(currentBoneIndex, weight);
            }
        }
        return bones;
    }

    unsigned int GetAllVertexCount(const aiScene& scene) const
    {
        uint32_t vertexCount{ 0 };
        for (uint32_t meshIndex = 0; meshIndex < scene.mNumMeshes; meshIndex++) {
            vertexCount += scene.mMeshes[meshIndex]->mNumVertices;
        };
        return vertexCount;
    }

    unsigned int ReadMeshes(const aiScene& scene, const FlagSet<AssimpMeshFactoryCreateFlags>& flags, VertexDataBuffer& inOutVertexBuffer, std::vector<glm::vec3>& inOutVertices, std::vector<uint32_t>& inOutIndices, std::vector<MeshPart>& inOutMeshParts) const
    {
        uint32_t allVertexCount = GetAllVertexCount(scene);
        std::vector<VertexBoneData> vertexBoneData;
        if (flags & AssimpMeshFactoryCreateFlags::ANIMATION) {
            vertexBoneData.resize(allVertexCount);

            uint32_t vertexBaseOffset = 0;
            for (uint32_t meshIndex = 0; meshIndex < scene.mNumMeshes; meshIndex++) {
                const aiMesh& assMesh = *scene.mMeshes[meshIndex];
                const auto vertexBonePart = LoadAnimationBones(assMesh, vertexBaseOffset);
                for (size_t j = 0; j < vertexBonePart.size(); j++) {
                    vertexBoneData[j + vertexBaseOffset] = vertexBonePart[j];
                }
                vertexBaseOffset += assMesh.mNumVertices;
            }
        }

        std::vector<glm::mat4> transforms;
        ReadNodeHierarchy(scene.mRootNode, AssimpGlmConvertor::ToGlmMat4(scene.mRootNode->mTransformation), transforms);

        uint32_t vertexBaseOffset = 0;
        uint32_t indexBaseOffset = 0;
        for (uint32_t meshIndex = 0; meshIndex < scene.mNumMeshes; meshIndex++) {
            const aiMesh& assMesh = *scene.mMeshes[meshIndex];
            ReadVertexData(assMesh, flags, vertexBoneData, vertexBaseOffset, inOutVertexBuffer, inOutVertices);

            std::vector<uint32_t> meshIndices;
            ReadIndices(assMesh, meshIndices);

            inOutIndices.insert(inOutIndices.end(), meshIndices.begin(), meshIndices.end());
            inOutMeshParts.push_back(MeshPart{ vertexBaseOffset, indexBaseOffset, static_cast<uint32_t>(meshIndices.size()), transforms.at(meshIndex), assMesh.mMaterialIndex });

            vertexBaseOffset += assMesh.mNumVertices;
            indexBaseOffset += static_cast<uint32_t>(meshIndices.size());
        }

        return allVertexCount;
    }
};

#endif