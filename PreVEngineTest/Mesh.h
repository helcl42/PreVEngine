#ifndef __MESH_H__
#define __MESH_H__

#include <Utils.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

using namespace PreVEngine;

enum class VertexLayoutComponent {
    FLOAT = 0x0,
    VEC2 = 0x1,
    VEC3 = 0x2,
    VEC4 = 0x3
};

struct VertexLayout {
private:
    std::vector<VertexLayoutComponent> m_components;

public:
    VertexLayout()
    {
    }

    VertexLayout(const std::vector<VertexLayoutComponent>& components)
        : m_components(components)
    {
    }

public:
    const std::vector<VertexLayoutComponent>& GetComponents() const
    {
        return m_components;
    }

    uint32_t GetStride() const
    {
        uint32_t singleVertexPackSizeInBytes = 0;
        for (auto& component : m_components) {
            switch (component) {
            case VertexLayoutComponent::FLOAT:
                singleVertexPackSizeInBytes += 1 * sizeof(float);
                break;
            case VertexLayoutComponent::VEC2:
                singleVertexPackSizeInBytes += 2 * sizeof(float);
                break;
            case VertexLayoutComponent::VEC4:
                singleVertexPackSizeInBytes += 4 * sizeof(float);
                break;
            default:
                singleVertexPackSizeInBytes += 3 * sizeof(float);
                break;
            }
        }
        return singleVertexPackSizeInBytes;
    }
};

class VertexDataBuffer {
private:
    std::vector<uint8_t> m_buffer;

public:
    VertexDataBuffer()
    {
    }

    VertexDataBuffer(const size_t initialSize)
    {
        m_buffer.reserve(initialSize);
    }

public:
    void Add(const void* data, const unsigned int size)
    {
        m_buffer.insert(m_buffer.end(), (const uint8_t*)data, (const uint8_t*)data + size);
    }

    void Add(float data)
    {
        Add(&data, sizeof(float));
    }

    void Add(const glm::vec2& data)
    {
        Add(&data, sizeof(glm::vec2));
    }

    void Add(const glm::vec3& data)
    {
        Add(&data, sizeof(glm::vec3));
    }

    void Add(const glm::vec4& data)
    {
        Add(&data, sizeof(glm::vec4));
    }

    void Reset()
    {
        m_buffer.clear();
    }

    const uint8_t* GetData() const
    {
        return m_buffer.data();
    }
};

class IMesh {
public:
    virtual const VertexLayout& GetVertextLayout() const = 0;

    virtual const void* GetVertices() const = 0;

    virtual uint32_t GerVerticesCount() const = 0;

    virtual const std::vector<uint32_t>& GerIndices() const = 0;

    virtual bool HasIndices() const = 0;

public:
    virtual ~IMesh()
    {
    }
};

class CubeMesh : public IMesh {
private:
    struct Vertex {
        glm::vec3 pos;
        glm::vec2 tc;
        glm::vec3 normal;
    };

private:
    const VertexLayout m_vertexLayout{ { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 } };

    const std::vector<Vertex> m_vertices = {
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

    const std::vector<uint32_t> m_indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };

public:
    const VertexLayout& GetVertextLayout() const override
    {
        return m_vertexLayout;
    }

    const void* GetVertices() const override
    {
        return (const void*)m_vertices.data();
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

class QuadMesh : public IMesh {
private:
    struct Vertex {
        glm::vec3 pos;
        glm::vec2 tc;
        glm::vec3 normal;
    };

private:
    const VertexLayout m_vertexLayout{ { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 } };

    const std::vector<Vertex> m_vertices = {
        { { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
        { { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
        { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
        { { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }
    };

    const std::vector<uint32_t> m_indices = {
        0, 1, 2, 2, 3, 0
    };

public:
    const VertexLayout& GetVertextLayout() const override
    {
        return m_vertexLayout;
    }

    const void* GetVertices() const override
    {
        return (const void*)m_vertices.data();
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

class PlaneMesh : public IMesh {
private:
    const VertexLayout m_vertexLayout{ { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 } };

    VertexDataBuffer m_vertexDataBuffer;

    uint32_t m_verticesCount = 0;

    std::vector<uint32_t> m_indices;

public:
    PlaneMesh(const float xSize, const float zSize, const uint32_t xDivs, const uint32_t zDivs, const float sMax = 1.0f, const float tMax = 1.0f)
    {
        const float x2 = xSize / 2.0f;
        const float z2 = zSize / 2.0f;
        const float iFactor = (float)zSize / zDivs;
        const float jFactor = (float)xSize / xDivs;
        const float texi = (float)sMax / zDivs;
        const float texj = (float)tMax / xDivs;

        for (uint32_t i = 0; i <= zDivs; i++) {
            float z = iFactor * i - z2;

            for (uint32_t j = 0; j <= xDivs; j++) {
                float x = jFactor * j - x2;

                glm::vec3 vertex(x, 0.0f, z);
                glm::vec2 textureCoord(j * texi, i * texj);
                glm::vec3 normal(0.0f, 1.0f, 0.0f);

                m_vertexDataBuffer.Add(vertex);
                m_vertexDataBuffer.Add(textureCoord);
                m_vertexDataBuffer.Add(normal);
                m_verticesCount++;
            }
        }

        for (uint32_t i = 0; i < zDivs; i++) {
            const uint32_t rowStart = i * (xDivs + 1);
            const uint32_t nextRowStart = (i + 1) * (xDivs + 1);

            for (uint32_t j = 0; j < xDivs; j++) {
                const uint32_t indices[] = {
                    rowStart + j,
                    nextRowStart + j,
                    nextRowStart + j + 1,
                    rowStart + j,
                    nextRowStart + j + 1,
                    rowStart + j + 1
                };

                for (const auto index : indices) {
                    m_indices.emplace_back(index);
                }
            }
        }
    }

    virtual ~PlaneMesh()
    {
    }

public:
    const VertexLayout& GetVertextLayout() const override
    {
        return m_vertexLayout;
    }

    const void* GetVertices() const override
    {
        return m_vertexDataBuffer.GetData();
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

class AssimpMeshFactory;

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

class AssimpMesh : public IMesh {
private:
    friend AssimpMeshFactory;

private:
    VertexLayout m_vertexLayout;

    VertexDataBuffer m_vertexDataBuffer;

    uint32_t m_verticesCount = 0;

    std::vector<uint32_t> m_indices;

public:
    const VertexLayout& GetVertextLayout() const override
    {
        return m_vertexLayout;
    }

    const void* GetVertices() const override
    {
        return m_vertexDataBuffer.GetData();
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

enum class AnimationStateType {
    RUNNING = 0,
    PAUSED,
    STOPPED
};

struct BoneInfo {
    glm::mat4 boneOffset;

    glm::mat4 finalTransformation;
};

#define NUM_BONES_PER_VEREX 4

struct VertexBoneData {
    unsigned int ids[NUM_BONES_PER_VEREX];

    float weights[NUM_BONES_PER_VEREX];

    VertexBoneData()
    {
        Reset();
    }

    ~VertexBoneData() = default;

    void Reset()
    {
        std::memset(ids, 0, sizeof(ids));
        std::memset(weights, 0, sizeof(weights));
    }

    void AddBoneData(unsigned int boneID, float weight)
    {
        unsigned int minWeightIndex = 0;
        float minWeight = std::numeric_limits<float>::max();
        for (unsigned int i = 0; i < ArraySize(ids); i++) {
            if (weights[i] < minWeight) {
                minWeight = weights[i];
                minWeightIndex = i;
            }
        }

        ids[minWeightIndex] = boneID;
        weights[minWeightIndex] = weight;

        float weightSum = 0.0f;
        for (unsigned int i = 0; i < ArraySize(ids); i++) {
            weightSum += weights[i];
        }

        for (unsigned int i = 0; i < ArraySize(ids); i++) {
            weights[i] /= weightSum;
        }
    }
};

class AnimatedAssimpMesh : public IMesh {
private:
    friend AssimpMeshFactory;

private:
    VertexLayout m_vertexLayout;

    VertexDataBuffer m_vertexDataBuffer;

    uint32_t m_verticesCount = 0;

    std::vector<uint32_t> m_indices;

    // Animation
    float m_elapsedTime;

    std::map<std::string, unsigned int> m_boneMapping; // maps a bone name to its index

    unsigned int m_numBones;

    std::vector<BoneInfo> m_boneInfos;

    glm::mat4 m_globalInverseTransform;

    std::vector<glm::mat4> m_boneTransforms;

    const aiScene* m_scene;

    Assimp::Importer m_importer;

    AnimationStateType m_animationState = AnimationStateType::RUNNING;

    unsigned int m_animationIndex = 0;

    float m_animationSpeed = 1.0f;

public:
    const VertexLayout& GetVertextLayout() const override
    {
        return m_vertexLayout;
    }

    const void* GetVertices() const override
    {
        return m_vertexDataBuffer.GetData();
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

    // Animation
    unsigned int FindPosition(const float animationTime, const aiNodeAnim* nodeAnimation) const
    {
        for (unsigned int i = 0; i < nodeAnimation->mNumPositionKeys - 1; i++) {
            if (animationTime < static_cast<float>(nodeAnimation->mPositionKeys[i + 1].mTime)) {
                return i;
            }
        }
        assert(0);
        return 0;
    }

    unsigned int FindRotation(const float animationTime, const aiNodeAnim* nodeAnimation) const
    {
        assert(nodeAnimation->mNumRotationKeys > 0);
        for (unsigned int i = 0; i < nodeAnimation->mNumRotationKeys - 1; i++) {
            if (animationTime < static_cast<float>(nodeAnimation->mRotationKeys[i + 1].mTime)) {
                return i;
            }
        }
        assert(0);
        return 0;
    }

    unsigned int FindScaling(const float animationTime, const aiNodeAnim* nodeAnimation) const
    {
        assert(nodeAnimation->mNumScalingKeys > 0);
        for (unsigned int i = 0; i < nodeAnimation->mNumScalingKeys - 1; i++) {
            if (animationTime < static_cast<float>(nodeAnimation->mScalingKeys[i + 1].mTime)) {
                return i;
            }
        }
        assert(0);
        return 0;
    }

    const aiNodeAnim* FindNodeAnimByName(const aiAnimation* animation, const std::string& nodeName) const
    {
        for (unsigned int i = 0; i < animation->mNumChannels; i++) {
            const auto nodeAnimation = animation->mChannels[i];
            if (std::string(nodeAnimation->mNodeName.data) == nodeName) {
                return nodeAnimation;
            }
        }
        return NULL;
    }

    aiVector3D CalculateInterpolatedPosition(const float animationTime, const aiNodeAnim* nodeAnimation) const
    {
        aiVector3D outVector;
        if (nodeAnimation->mNumPositionKeys == 1) {
            outVector = nodeAnimation->mPositionKeys[0].mValue;
        } else {
            const unsigned int positionIndex = FindPosition(animationTime, nodeAnimation);
            const unsigned int nextPositionIndex = (positionIndex + 1);
            assert(nextPositionIndex < nodeAnimation->mNumPositionKeys);
            const float deltaTime = static_cast<float>(nodeAnimation->mPositionKeys[nextPositionIndex].mTime - nodeAnimation->mPositionKeys[positionIndex].mTime);
            const float factor = (animationTime - static_cast<float>(nodeAnimation->mPositionKeys[positionIndex].mTime)) / deltaTime;
            assert(factor >= 0.0f && factor <= 1.0f);
            const auto& start = nodeAnimation->mPositionKeys[positionIndex].mValue;
            const auto& end = nodeAnimation->mPositionKeys[nextPositionIndex].mValue;
            const auto delta = end - start;
            outVector = start + factor * delta;
        }
        return outVector;
    }

    aiQuaternion CalculateInterpolatedRotation(const float animationTime, const aiNodeAnim* nodeAnimation) const
    {
        aiQuaternion outQuanternion;
        if (nodeAnimation->mNumRotationKeys == 1) {
            outQuanternion = nodeAnimation->mRotationKeys[0].mValue;
        } else {
            const auto rotationIndex = FindRotation(animationTime, nodeAnimation);
            const auto nextRotationIndex = (rotationIndex + 1);
            assert(nextRotationIndex < nodeAnimation->mNumRotationKeys);
            const float deltaTime = static_cast<float>(nodeAnimation->mRotationKeys[nextRotationIndex].mTime - nodeAnimation->mRotationKeys[rotationIndex].mTime);
            const float factor = (animationTime - static_cast<float>(nodeAnimation->mRotationKeys[rotationIndex].mTime)) / deltaTime;
            assert(factor >= 0.0f && factor <= 1.0f);
            const auto& startRotationQ = nodeAnimation->mRotationKeys[rotationIndex].mValue;
            const auto& endRotationQ = nodeAnimation->mRotationKeys[nextRotationIndex].mValue;
            aiQuaternion::Interpolate(outQuanternion, startRotationQ, endRotationQ, factor);
            outQuanternion = outQuanternion.Normalize();
        }
        return outQuanternion;
    }

    aiVector3D CalculateInterpolatedScaling(const float animationTime, const aiNodeAnim* nodeAnimation) const
    {
        aiVector3D outVector;
        if (nodeAnimation->mNumScalingKeys == 1) {
            outVector = nodeAnimation->mScalingKeys[0].mValue;
        } else {
            const auto scalingIndex = FindScaling(animationTime, nodeAnimation);
            const auto nextScalingIndex = (scalingIndex + 1);
            assert(nextScalingIndex < nodeAnimation->mNumScalingKeys);
            const float deltaTime = static_cast<float>(nodeAnimation->mScalingKeys[nextScalingIndex].mTime - nodeAnimation->mScalingKeys[scalingIndex].mTime);
            const float factor = (animationTime - static_cast<float>(nodeAnimation->mScalingKeys[scalingIndex].mTime)) / deltaTime;
            assert(factor >= 0.0f && factor <= 1.0f);
            const auto& start = nodeAnimation->mScalingKeys[scalingIndex].mValue;
            const auto& end = nodeAnimation->mScalingKeys[nextScalingIndex].mValue;
            const auto delta = end - start;
            outVector = start + factor * delta;
        }
        return outVector;
    }

    void UpdateNodeHeirarchy(const float animationTime, const aiNode* node, const glm::mat4& parentTransformation)
    {
        const std::string nodeName{ node->mName.data };
        const auto currentAnimation = m_scene->mAnimations[m_animationIndex];
        auto nodeTransformation = AssimpGlmConvertor::ToGlmMat4(node->mTransformation);

        const auto nodeAnimation = FindNodeAnimByName(currentAnimation, nodeName);
        if (nodeAnimation) {
            const auto scaling = CalculateInterpolatedScaling(animationTime, nodeAnimation);
            const auto scaleMatrix = glm::scale(glm::mat4(1.0), glm::vec3(scaling.x, scaling.y, scaling.z));

            const auto rotationQuat = CalculateInterpolatedRotation(animationTime, nodeAnimation);
            const auto rotationMatrix = AssimpGlmConvertor::ToGlmMat4(rotationQuat.GetMatrix());

            const auto translation = CalculateInterpolatedPosition(animationTime, nodeAnimation);
            const auto translationMatrix = glm::translate(glm::mat4(1.0), glm::vec3(translation.x, translation.y, translation.z));

            nodeTransformation = translationMatrix * rotationMatrix * scaleMatrix;
        }

        auto globalTransformation = parentTransformation * nodeTransformation;
        if (m_boneMapping.find(nodeName) != m_boneMapping.end()) {
            const auto boneIndex = m_boneMapping[nodeName];
            m_boneInfos[boneIndex].finalTransformation = m_globalInverseTransform * globalTransformation * m_boneInfos[boneIndex].boneOffset;
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            UpdateNodeHeirarchy(animationTime, node->mChildren[i], globalTransformation);
        }
    }

    void Update(float deltaTime)
    {
        deltaTime *= m_animationSpeed;

        m_animationIndex %= m_scene->mNumAnimations;

        if (m_animationState == AnimationStateType::RUNNING) {
            m_elapsedTime += deltaTime;
        } else if (m_animationState == AnimationStateType::STOPPED) {
            m_elapsedTime = 0.0f;
        }

        const auto currentAnimation = m_scene->mAnimations[m_animationIndex];
        const auto ticksPerSecond = static_cast<float>(currentAnimation->mTicksPerSecond != 0 ? currentAnimation->mTicksPerSecond : 25.0f);
        const auto timeInTicks = m_elapsedTime * ticksPerSecond;
        const auto animationTime = fmod(timeInTicks, static_cast<float>(currentAnimation->mDuration));

        UpdateNodeHeirarchy(animationTime, m_scene->mRootNode, glm::mat4(1.0f));

        m_boneTransforms.resize(m_numBones);
        for (unsigned int i = 0; i < m_numBones; i++) {
            m_boneTransforms[i] = m_boneInfos[i].finalTransformation;
        }
    }

    const std::vector<glm::mat4>& GetBoneTransforms() const
    {
        return m_boneTransforms;
    }

    void SetAnimationState(const AnimationStateType animationState)
    {
        m_animationState = animationState;
    }

    AnimationStateType GetAnimationState() const
    {
        return m_animationState;
    }

    void SetAnimationIndex(const unsigned int index)
    {
        m_animationIndex = index;
    }

    unsigned int GetAnimationIndex() const
    {
        return m_animationIndex;
    }

    void SetAnimationSpeed(const float speed)
    {
        m_animationSpeed = speed;
    }

    float GetAnimationSpeed() const
    {
        return m_animationSpeed;
    }

    void SetAnimationTime(const float elapsed)
    {
        m_elapsedTime = elapsed;
    }

    float GetAnimationTime() const
    {
        return m_elapsedTime;
    }
};

class AssimpMeshFactory {
public:
    enum class AssimpMeshFactoryCreateFlags {
        Animation,
        TangentBitangent,
        _
    };

public:
    std::shared_ptr<IMesh> CreateMesh(const std::string& modelPath, const FlagSet<AssimpMeshFactoryCreateFlags>& flags = FlagSet<AssimpMeshFactory::AssimpMeshFactoryCreateFlags>{}) const
    {
        Assimp::Importer importer{};
        const aiScene* scene = importer.ReadFile(modelPath, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_GenSmoothNormals | aiProcess_FixInfacingNormals | aiProcess_FindInvalidData);
        if (!scene) {
            throw std::runtime_error("Could not load model: " + modelPath);
        }

        std::shared_ptr<AssimpMesh> mesh = std::make_shared<AssimpMesh>();

        mesh->m_vertexLayout = GetVertexLayout(flags);

        ReadMeshes(*scene, flags, mesh);

        return mesh;
    }

private:
    VertexLayout GetVertexLayout(const FlagSet<AssimpMeshFactoryCreateFlags>& flags) const
    {
        if (flags & AssimpMeshFactoryCreateFlags::Animation && flags & AssimpMeshFactoryCreateFlags::TangentBitangent) {
            return { { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 } };
        } else if (flags & AssimpMeshFactoryCreateFlags::Animation) {
            return { { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4 } };
        } else if (flags & AssimpMeshFactoryCreateFlags::TangentBitangent) {
            return { { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 } };
        } else {
            return { { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 } };
        }
    }

    void ReadIndices(const aiMesh& mesh, std::shared_ptr<AssimpMesh>& inOutMesh) const
    {
        for (unsigned int i = 0; i < mesh.mNumFaces; i++) {
            const aiFace& face = mesh.mFaces[i];
            for (unsigned int k = 0; k < face.mNumIndices; k++) {
                inOutMesh->m_indices.emplace_back(face.mIndices[k]);
            }
        }
    }

    void AddDefaultVertexData(const aiMesh& mesh, const unsigned int vertexIndex, std::shared_ptr<AssimpMesh>& inOutMesh) const
    {
        glm::vec3 pos = glm::make_vec3(&mesh.mVertices[vertexIndex].x);
        glm::vec2 uv = mesh.mTextureCoords[0] != NULL ? glm::make_vec3(&mesh.mTextureCoords[0][vertexIndex].x) : glm::vec2(1.0f, 1.0f);
        glm::vec3 normal = mesh.HasNormals() ? glm::make_vec3(&mesh.mNormals[vertexIndex].x) : glm::vec3(0.0f, 1.0f, 0.0f);

        inOutMesh->m_vertexDataBuffer.Add(&pos, sizeof(glm::vec3));
        inOutMesh->m_vertexDataBuffer.Add(&uv, sizeof(glm::vec2));
        inOutMesh->m_vertexDataBuffer.Add(&normal, sizeof(glm::vec3));
    }

    void AddAnimationData(const std::vector<VertexBoneData>& vertexBoneData, const unsigned int vertexIndex, std::shared_ptr<AssimpMesh>& inOutMesh) const
    {
        const auto& singleVertexBoneData = vertexBoneData[vertexIndex];

        inOutMesh->m_vertexDataBuffer.Add(&singleVertexBoneData.ids, static_cast<unsigned int>(ArraySize(singleVertexBoneData.ids)));
        inOutMesh->m_vertexDataBuffer.Add(&singleVertexBoneData.weights, static_cast<unsigned int>(ArraySize(singleVertexBoneData.weights)));
    }

    void AddBumpMappingData(const aiMesh& mesh, const unsigned int vertexIndex, std::shared_ptr<AssimpMesh>& inOutMesh) const
    {
        glm::vec3 tangent = glm::make_vec3(&mesh.mTangents[vertexIndex].x);
        glm::vec3 biTangent = glm::make_vec3(&mesh.mBitangents[vertexIndex].x);
        inOutMesh->m_vertexDataBuffer.Add(&tangent, sizeof(glm::vec3));
        inOutMesh->m_vertexDataBuffer.Add(&biTangent, sizeof(glm::vec3));
    }

    void ReadVertexData(const aiMesh& mesh, const FlagSet<AssimpMeshFactoryCreateFlags>& flags, const std::vector<VertexBoneData>& vertexBoneData, const unsigned int vertexBaseOffset, std::shared_ptr<AssimpMesh>& inOutMesh) const
    {
        for (unsigned int i = 0; i < mesh.mNumVertices; i++) {
            AddDefaultVertexData(mesh, i, inOutMesh);

            if (flags & AssimpMeshFactoryCreateFlags::Animation) {
                AddAnimationData(vertexBoneData, vertexBaseOffset + i, inOutMesh);
            }

            if (flags & AssimpMeshFactoryCreateFlags::TangentBitangent) {
                AddBumpMappingData(mesh, i, inOutMesh);
            }

            inOutMesh->m_verticesCount++;
        }
    }

    std::vector<VertexBoneData> LoadAnimationBones(const aiMesh& mesh, const unsigned int vertexBaseOffset) const
    {
        std::vector<VertexBoneData> bones(mesh.mNumVertices);
        std::map<std::string, unsigned int> boneMapping;

        for (unsigned int i = 0; i < mesh.mNumBones; i++) {
            const std::string currentBoneName{ mesh.mBones[i]->mName.data };

            unsigned int currentBoneIndex = 0;
            if (boneMapping.find(currentBoneName) == boneMapping.end()) {
                currentBoneIndex = static_cast<unsigned int>(boneMapping.size());
                boneMapping[currentBoneName] = currentBoneIndex;
            } else {
                currentBoneIndex = boneMapping[currentBoneName];
            }

            for (unsigned int j = 0; j < mesh.mBones[i]->mNumWeights; j++) {
                const unsigned int vertexId = vertexBaseOffset + mesh.mBones[i]->mWeights[j].mVertexId;
                const float weight = mesh.mBones[i]->mWeights[j].mWeight;
                bones[vertexId].AddBoneData(currentBoneIndex, weight);
            }
        }
        return bones;
    }

    unsigned int GetAllVertexCount(const aiScene& scene) const
    {
        unsigned int vertexCount{ 0 };
        for (unsigned int i = 0; i < scene.mNumMeshes; i++) {
            vertexCount += scene.mMeshes[i]->mNumVertices;
        };
        return vertexCount;
    }

    void ReadMeshes(const aiScene& scene, const FlagSet<AssimpMeshFactoryCreateFlags>& flags, std::shared_ptr<AssimpMesh>& inOutMesh) const
    {
        std::vector<VertexBoneData> vertexBoneData;
        if (flags & AssimpMeshFactoryCreateFlags::Animation) {
            unsigned int allVertexCount = GetAllVertexCount(scene);
            vertexBoneData.resize(allVertexCount);

            unsigned int vertexBaseOffset = 0;
            for (unsigned int i = 0; i < scene.mNumMeshes; i++) {
                const aiMesh& assMesh = *scene.mMeshes[i];
                vertexBoneData = LoadAnimationBones(assMesh, vertexBaseOffset);
                vertexBaseOffset += assMesh.mNumVertices;
            }            
        }

        unsigned int vertexBaseOffset = 0;
        for (unsigned int i = 0; i < scene.mNumMeshes; i++) {
            const aiMesh& assMesh = *scene.mMeshes[i];
            ReadVertexData(assMesh, flags, vertexBoneData, vertexBaseOffset, inOutMesh);
            ReadIndices(assMesh, inOutMesh);
            vertexBaseOffset += assMesh.mNumVertices;
        }
    }
};

#endif