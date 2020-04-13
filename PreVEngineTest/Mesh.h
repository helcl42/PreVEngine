#ifndef __MESH_H__
#define __MESH_H__

#include <Utils.h>

#include "AssimpCommon.h"

class CubeMesh : public IMesh {
private:
    struct Vertex {
        glm::vec3 pos;
        glm::vec2 tc;
        glm::vec3 normal;
    };

private:
    static const inline VertexLayout vertexLayout{ { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 } };

    static const inline std::vector<Vertex> vertices = {
        // FROMT
        { { -0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
        { { 0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
        { { 0.5f, 0.5f, 0.5f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
        { { -0.5f, 0.5f, 0.5f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },

        // BACK
        { { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },
        { { 0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },
        { { 0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },
        { { -0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },

        // TOP
        { { -0.5f, 0.5f, 0.5f }, { 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
        { { 0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
        { { 0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } },
        { { -0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } },

        // BOTTOM
        { { -0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },
        { { 0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },
        { { 0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f }, { 0.0f, -1.0f, 0.0f } },
        { { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f }, { 0.0f, -1.0f, 0.0f } },

        // LEFT
        { { -0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f } },
        { { -0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f } },
        { { -0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f } },
        { { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f }, { -1.0f, 0.0f, 0.0f } },

        // RIGHT
        { { 0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
        { { 0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
        { { 0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f } },
        { { 0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f } }
    };

    static const inline std::vector<uint32_t> indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };

public:
    const VertexLayout& GetVertexLayout() const override
    {
        return vertexLayout;
    }

    const void* GetVertexData() const override
    {
        return (const void*)vertices.data();
    }
    
    std::vector<glm::vec3> GetVertices() const override
    {
        std::vector<glm::vec3> verts{ vertices.size() };
        for (auto i = 0; i < vertices.size(); i++) {
            verts[i] = vertices[i].pos;
        }
        return verts;
    }

    uint32_t GerVerticesCount() const override
    {
        return static_cast<uint32_t>(vertices.size());
    }

    const std::vector<uint32_t>& GerIndices() const override
    {
        return indices;
    }

    bool HasIndices() const override
    {
        return indices.size() > 0;
    }
};

class QuadMesh : public IMesh {
private:
    struct Vertex {
        glm::vec3 pos;
        glm::vec2 tc;
        glm::vec3 normal;
    };

private:
    static const inline VertexLayout vertexLayout{ { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 } };

    static const inline std::vector<Vertex> vertices = {
        { { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
        { { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
        { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
        { { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }
    };

    static const inline std::vector<uint32_t> indices = {
        0, 1, 2, 2, 3, 0
    };

public:
    const VertexLayout& GetVertexLayout() const override
    {
        return vertexLayout;
    }

    const void* GetVertexData() const override
    {
        return (const void*)vertices.data();
    }

    std::vector<glm::vec3> GetVertices() const override
    {
        std::vector<glm::vec3> verts{ vertices.size() };
        for (auto i = 0; i < vertices.size(); i++) {
            verts[i] = vertices[i].pos;
        }
        return verts;
    }

    uint32_t GerVerticesCount() const override
    {
        return static_cast<uint32_t>(vertices.size());
    }

    const std::vector<uint32_t>& GerIndices() const override
    {
        return indices;
    }

    bool HasIndices() const override
    {
        return indices.size() > 0;
    }
};

class PlaneMesh : public IMesh {
private:
    VertexLayout m_vertexLayout;

    VertexDataBuffer m_vertexDataBuffer;

    std::vector<glm::vec3> m_vertices;

    std::vector<uint32_t> m_indices;

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
    }

    virtual ~PlaneMesh() = default;

public:
    const VertexLayout& GetVertexLayout() const override
    {
        return m_vertexLayout;
    }

    const void* GetVertexData() const override
    {
        return m_vertexDataBuffer.GetData();
    }

    std::vector<glm::vec3> GetVertices() const override
    {
        return m_vertices;
    }

    uint32_t GerVerticesCount() const override
    {
        return static_cast<uint32_t>(m_vertices.size());
    }

    const std::vector<uint32_t>& GerIndices() const override
    {
        return m_indices;
    }

    bool HasIndices() const override
    {
        return m_indices.size() > 0;
    }
};

class SphereMesh : public IMesh {
private:
    VertexLayout m_vertexLayout;

    VertexDataBuffer m_vertexDataBuffer;

    std::vector<glm::vec3> m_vertices;

    std::vector<uint32_t> m_indices;

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

        float addAngleY{ -degreesHorizontal / static_cast<float>(subDivY) };
        float addAngleZ{ degreesVertical / static_cast<float>(subDivZ) };
        float curAngleY{ 0.0f };
        int stepsY{ 1 };

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
                indexBase += 6;

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
    }

    virtual ~SphereMesh() = default;

public:
    const VertexLayout& GetVertexLayout() const override
    {
        return m_vertexLayout;
    }

    const void* GetVertexData() const override
    {
        return m_vertexDataBuffer.GetData();
    }

    std::vector<glm::vec3> GetVertices() const override
    {
        return m_vertices;
    }

    uint32_t GerVerticesCount() const override
    {
        return static_cast<uint32_t>(m_vertices.size());
    }

    const std::vector<uint32_t>& GerIndices() const override
    {
        return m_indices;
    }

    bool HasIndices() const override
    {
        return m_indices.size() > 0;
    }
};

class MeshFactory;

class AssimpGlmConvertor {
public:
    static glm::mat4 ToGlmMat4(const aiMatrix3x3& inputMatrix)
    {
        glm::mat4 newMatrix;
        newMatrix[0][0] = inputMatrix.a1;
        newMatrix[1][0] = inputMatrix.a2;
        newMatrix[2][0] = inputMatrix.a3;
        newMatrix[3][0] = 0.0f;
        newMatrix[0][1] = inputMatrix.b1;
        newMatrix[1][1] = inputMatrix.b2;
        newMatrix[2][1] = inputMatrix.b3;
        newMatrix[3][1] = 0.0f;
        newMatrix[0][2] = inputMatrix.c1;
        newMatrix[1][2] = inputMatrix.c2;
        newMatrix[2][2] = inputMatrix.c3;
        newMatrix[3][2] = 0.0f;
        newMatrix[0][3] = 0.0f;
        newMatrix[1][3] = 0.0f;
        newMatrix[2][3] = 0.0f;
        newMatrix[3][3] = 1.0f;
        return newMatrix;
    }

    static glm::mat4 ToGlmMat4(const aiMatrix4x4& inputMatrix)
    {
        glm::mat4 newMatrix;
        newMatrix[0][0] = inputMatrix.a1;
        newMatrix[1][0] = inputMatrix.a2;
        newMatrix[2][0] = inputMatrix.a3;
        newMatrix[3][0] = inputMatrix.a4;
        newMatrix[0][1] = inputMatrix.b1;
        newMatrix[1][1] = inputMatrix.b2;
        newMatrix[2][1] = inputMatrix.b3;
        newMatrix[3][1] = inputMatrix.b4;
        newMatrix[0][2] = inputMatrix.c1;
        newMatrix[1][2] = inputMatrix.c2;
        newMatrix[2][2] = inputMatrix.c3;
        newMatrix[3][2] = inputMatrix.c4;
        newMatrix[0][3] = inputMatrix.d1;
        newMatrix[1][3] = inputMatrix.d2;
        newMatrix[2][3] = inputMatrix.d3;
        newMatrix[3][3] = inputMatrix.d4;
        return newMatrix;
    }

    static glm::vec3 ToGlmVec3(const aiVector3D& inputVector)
    {
        return glm::vec3(inputVector.x, inputVector.y, inputVector.z);
    }

    static glm::vec2 ToGlmVec2(const aiVector2D& inputVector)
    {
        return glm::vec2(inputVector.x, inputVector.y);
    }
};

class ModelMesh : public IMesh {
private:
    friend MeshFactory;

private:
    VertexLayout m_vertexLayout;

    VertexDataBuffer m_vertexDataBuffer;

    std::vector<glm::vec3> m_vertices;

    uint32_t m_verticesCount = 0;

    std::vector<uint32_t> m_indices;

public:
    const VertexLayout& GetVertexLayout() const override
    {
        return m_vertexLayout;
    }

    const void* GetVertexData() const override
    {
        return m_vertexDataBuffer.GetData();
    }

    std::vector<glm::vec3> GetVertices() const override
    {
        return m_vertices;
    }

    uint32_t GerVerticesCount() const override
    {
        return m_verticesCount;
    }

    const std::vector<uint32_t>& GerIndices() const override
    {
        return m_indices;
    }

    bool HasIndices() const override
    {
        return m_indices.size() > 0;
    }
};

class MeshFactory {
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
        mesh->m_verticesCount = ReadMeshes(*scene, flags, mesh->m_vertexDataBuffer, mesh->m_vertices, mesh->m_indices);

        return mesh;
    }

private:
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

    void AddDefaultVertexData(const aiMesh& mesh, const unsigned int vertexIndex, VertexDataBuffer& inOutVertexBuffer, std::vector<glm::vec3>& inOutVertices) const
    {
        glm::vec3 pos = glm::make_vec3(&mesh.mVertices[vertexIndex].x);
        glm::vec2 uv = mesh.mTextureCoords[0] != nullptr ? glm::make_vec3(&mesh.mTextureCoords[0][vertexIndex].x) : glm::vec2(1.0f, 1.0f);
        glm::vec3 normal = mesh.HasNormals() ? glm::make_vec3(&mesh.mNormals[vertexIndex].x) : glm::vec3(0.0f, 1.0f, 0.0f);

        inOutVertices.push_back(pos);

        inOutVertexBuffer.Add(pos);
        inOutVertexBuffer.Add(uv);
        inOutVertexBuffer.Add(normal);
    }

    void AddAnimationData(const std::vector<VertexBoneData>& vertexBoneData, const unsigned int vertexIndex, VertexDataBuffer& inOutVertexBuffer) const
    {
        const auto& singleVertexBoneData = vertexBoneData[vertexIndex];

        inOutVertexBuffer.Add(&singleVertexBoneData.ids, static_cast<unsigned int>(ArraySize(singleVertexBoneData.ids) * sizeof(unsigned int)));
        inOutVertexBuffer.Add(&singleVertexBoneData.weights, static_cast<unsigned int>(ArraySize(singleVertexBoneData.weights) * sizeof(float)));
    }

    void AddBumpMappingData(const aiMesh& mesh, const unsigned int vertexIndex, VertexDataBuffer& inOutVertexBuffer) const
    {
        glm::vec3 tangent = glm::make_vec3(&mesh.mTangents[vertexIndex].x);
        glm::vec3 biTangent = glm::make_vec3(&mesh.mBitangents[vertexIndex].x);
        inOutVertexBuffer.Add(tangent);
        inOutVertexBuffer.Add(biTangent);
    }

    void ReadVertexData(const aiMesh& mesh, const FlagSet<AssimpMeshFactoryCreateFlags>& flags, const std::vector<VertexBoneData>& vertexBoneData, const unsigned int vertexBaseOffset, VertexDataBuffer& inOutVertexBuffer, std::vector<glm::vec3>& inOutVertices) const
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

    std::vector<VertexBoneData> LoadAnimationBones(const aiMesh& mesh, const unsigned int vertexBaseOffset) const
    {
        std::vector<VertexBoneData> bones(mesh.mNumVertices);
        std::map<std::string, unsigned int> boneMapping;

        for (unsigned int boneIndex = 0; boneIndex < mesh.mNumBones; boneIndex++) {
            const std::string currentBoneName{ mesh.mBones[boneIndex]->mName.data };

            unsigned int currentBoneIndex = 0;
            if (boneMapping.find(currentBoneName) == boneMapping.end()) {
                currentBoneIndex = static_cast<unsigned int>(boneMapping.size());
                boneMapping[currentBoneName] = currentBoneIndex;
            } else {
                currentBoneIndex = boneMapping[currentBoneName];
            }

            for (unsigned int j = 0; j < mesh.mBones[boneIndex]->mNumWeights; j++) {
                const unsigned int vertexId = vertexBaseOffset + mesh.mBones[boneIndex]->mWeights[j].mVertexId;
                const float weight = mesh.mBones[boneIndex]->mWeights[j].mWeight;
                bones[vertexId].AddBoneData(currentBoneIndex, weight);
            }
        }
        return bones;
    }

    unsigned int GetAllVertexCount(const aiScene& scene) const
    {
        unsigned int vertexCount{ 0 };
        for (unsigned int meshIndex = 0; meshIndex < scene.mNumMeshes; meshIndex++) {
            vertexCount += scene.mMeshes[meshIndex]->mNumVertices;
        };
        return vertexCount;
    }

    unsigned int ReadMeshes(const aiScene& scene, const FlagSet<AssimpMeshFactoryCreateFlags>& flags, VertexDataBuffer& inOutVertexBuffer, std::vector<glm::vec3>& inOutVertices, std::vector<uint32_t>& inOutIndices) const
    {
        unsigned int allVertexCount = GetAllVertexCount(scene);
        std::vector<VertexBoneData> vertexBoneData;
        if (flags & AssimpMeshFactoryCreateFlags::ANIMATION) {
            vertexBoneData.resize(allVertexCount);

            unsigned int vertexBaseOffset = 0;
            for (unsigned int meshIndex = 0; meshIndex < scene.mNumMeshes; meshIndex++) {
                const aiMesh& assMesh = *scene.mMeshes[meshIndex];
                const auto vertexBonePart = LoadAnimationBones(assMesh, vertexBaseOffset);
                for (size_t j = 0; j < vertexBonePart.size(); j++) {
                    vertexBoneData[j + vertexBaseOffset] = vertexBonePart[j];
                }
                vertexBaseOffset += assMesh.mNumVertices;
            }
        }

        unsigned int vertexBaseOffset = 0;
        for (unsigned int meshIndex = 0; meshIndex < scene.mNumMeshes; meshIndex++) {
            const aiMesh& assMesh = *scene.mMeshes[meshIndex];
            ReadVertexData(assMesh, flags, vertexBoneData, vertexBaseOffset, inOutVertexBuffer, inOutVertices);
            ReadIndices(assMesh, inOutIndices);
            vertexBaseOffset += assMesh.mNumVertices;
        }

        return allVertexCount;
    }
};

#endif