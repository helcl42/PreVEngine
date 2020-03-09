#ifndef __MESH_H__
#define __MESH_H__

#include <Utils.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

using namespace PreVEngine;

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

struct VertexBoneData {
    inline static const int BONES_PER_VERTEX_COUNT = 4;

    unsigned int ids[BONES_PER_VERTEX_COUNT];

    float weights[BONES_PER_VERTEX_COUNT];

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

    void AddBoneData(const unsigned int boneId, const float weight)
    {
        unsigned int minWeightIndex = 0;
        float minWeight = std::numeric_limits<float>::max();
        for (unsigned int i = 0; i < ArraySize(ids); i++) {
            if (weights[i] < minWeight) {
                minWeight = weights[i];
                minWeightIndex = i;
            }
        }

        if (minWeight < weight) {
            ids[minWeightIndex] = boneId;
            weights[minWeightIndex] = weight;

            float weightSum = 0.0f;
            for (unsigned int i = 0; i < ArraySize(ids); i++) {
                weightSum += weights[i];
            }

            for (unsigned int i = 0; i < ArraySize(ids); i++) {
                weights[i] /= weightSum;
            }
        }
    }
};

class AnimationFactory;

class Animation : public IAnimation {
private:
    friend AnimationFactory;

private:
    struct BoneInfo {
        glm::mat4 boneOffset;

        glm::mat4 finalTransformation;
    };

private:
    float m_elapsedTime{ 0.0f };

    std::map<std::string, unsigned int> m_boneMapping; // maps a bone name to its index

    unsigned int m_numBones{ 0 };

    std::vector<BoneInfo> m_boneInfos;

    glm::mat4 m_globalInverseTransform{ glm::mat4(1.0f) };

    std::vector<glm::mat4> m_boneTransforms;

    const aiScene* m_scene{ nullptr };

    Assimp::Importer m_importer; // TODO get rid of that

    AnimationState m_animationState{ AnimationState::RUNNING };

    unsigned int m_animationIndex{ 0 };

    float m_animationSpeed{ 1.0f };

public:
    Animation() = default;

    ~Animation() = default;

private:
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
        return nullptr;
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

public:
    void Update(const float deltaTime) override
    {
        m_animationIndex %= m_scene->mNumAnimations;

        const float scaledDeltaTime = deltaTime * m_animationSpeed;
        const auto currentAnimation = m_scene->mAnimations[m_animationIndex];
        const float animationDuration = static_cast<float>(currentAnimation->mDuration);

        if (m_animationState == AnimationState::RUNNING) {
            m_elapsedTime += scaledDeltaTime;
            if (m_elapsedTime < 0.0f) {
                m_elapsedTime += animationDuration;
            }
        } else if (m_animationState == AnimationState::STOPPED) {
            m_elapsedTime = 0.0f;
        }

        const auto ticksPerSecond = static_cast<float>(currentAnimation->mTicksPerSecond != 0 ? currentAnimation->mTicksPerSecond : 25.0f);
        const auto timeInTicks = m_elapsedTime * ticksPerSecond;
        const auto animationTime = fmod(timeInTicks, animationDuration);

        UpdateNodeHeirarchy(animationTime, m_scene->mRootNode, glm::mat4(1.0f));

        m_boneTransforms.resize(m_numBones);
        for (unsigned int i = 0; i < m_numBones; i++) {
            m_boneTransforms[i] = m_boneInfos[i].finalTransformation;
        }
    }

    const std::vector<glm::mat4>& GetBoneTransforms() const override
    {
        return m_boneTransforms;
    }

    void SetState(const AnimationState animationState) override
    {
        m_animationState = animationState;
    }

    AnimationState GetState() const override
    {
        return m_animationState;
    }

    void SetIndex(const unsigned int index) override
    {
        m_animationIndex = index;
    }

    unsigned int GetIndex() const override
    {
        return m_animationIndex;
    }

    void SetSpeed(const float speed) override
    {
        m_animationSpeed = speed;
    }

    float GetSpeed() const override
    {
        return m_animationSpeed;
    }

    void SetTime(const float elapsed) override
    {
        m_elapsedTime = elapsed;
    }

    float GetTime() const override
    {
        return m_elapsedTime;
    }
};

class AssimpSceneLoader {
public:
    bool LoadScene(const std::string& modelPath, Assimp::Importer* importer, const aiScene** scene)
    {
#if defined(__ANDROID__)
        AAsset* asset = android_open_asset(modelPath.c_str(), AASSET_MODE_STREAMING);
        assert(asset);
        size_t size = AAsset_getLength(asset);

        assert(size > 0);

        void* meshData = malloc(size);
        AAsset_read(asset, meshData, size);
        AAsset_close(asset);

        *scene = importer->ReadFileFromMemory(meshData, size, 0);

        free(meshData);
#else
        *scene = importer->ReadFile(modelPath, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_GenSmoothNormals | aiProcess_FixInfacingNormals | aiProcess_FindInvalidData);
#endif
        if (!*scene) {
            return false;
        }
        return true;
    }
};

class AnimationFactory {
public:
    std::unique_ptr<IAnimation> CreateAnimation(const std::string& modelPath) const
    {
        // TODO copy asiimp animations to some local structure to avoid storing importer and whole scene !!!
        std::unique_ptr<Animation> animation = std::make_unique<Animation>();

        AssimpSceneLoader assimpSceneLoader{};
        if (!assimpSceneLoader.LoadScene(modelPath, &animation->m_importer, &animation->m_scene)) {
            throw std::runtime_error("Could not load model: " + modelPath);
        }

        animation->m_globalInverseTransform = glm::inverse(AssimpGlmConvertor::ToGlmMat4(animation->m_scene->mRootNode->mTransformation));

        for (unsigned int meshIndex = 0; meshIndex < animation->m_scene->mNumMeshes; meshIndex++) {
            const auto& mesh = *animation->m_scene->mMeshes[meshIndex];
            for (unsigned int i = 0; i < mesh.mNumBones; i++) {
                const std::string boneName{ mesh.mBones[i]->mName.data };

                unsigned int boneIndex;
                if (animation->m_boneMapping.find(boneName) == animation->m_boneMapping.end()) {
                    boneIndex = animation->m_numBones;
                    animation->m_numBones++;
                    animation->m_boneInfos.push_back(Animation::BoneInfo{});
                    animation->m_boneInfos[boneIndex].boneOffset = AssimpGlmConvertor::ToGlmMat4(mesh.mBones[i]->mOffsetMatrix);
                    animation->m_boneMapping[boneName] = boneIndex;
                } else {
                    boneIndex = animation->m_boneMapping[boneName];
                }
            }
        }

        return animation;
    }
};

class MeshFactory {
public:
    enum class AssimpMeshFactoryCreateFlags {
        ANIMATION,
        BUMP_MAPPING,
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

        std::unique_ptr<ModelMesh> mesh = std::make_unique<ModelMesh>();

        mesh->m_vertexLayout = GetVertexLayout(flags);
        mesh->m_verticesCount = ReadMeshes(*scene, flags, mesh->m_vertexDataBuffer, mesh->m_indices);

        return mesh;
    }

private:
    VertexLayout GetVertexLayout(const FlagSet<AssimpMeshFactoryCreateFlags>& flags) const
    {
        if (flags & AssimpMeshFactoryCreateFlags::ANIMATION && flags & AssimpMeshFactoryCreateFlags::BUMP_MAPPING) {
            return { { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 } };
        } else if (flags & AssimpMeshFactoryCreateFlags::ANIMATION) {
            return { { VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4 } };
        } else if (flags & AssimpMeshFactoryCreateFlags::BUMP_MAPPING) {
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

    void AddDefaultVertexData(const aiMesh& mesh, const unsigned int vertexIndex, VertexDataBuffer& inOutVertexBuffer) const
    {
        glm::vec3 pos = glm::make_vec3(&mesh.mVertices[vertexIndex].x);
        glm::vec2 uv = mesh.mTextureCoords[0] != nullptr ? glm::make_vec3(&mesh.mTextureCoords[0][vertexIndex].x) : glm::vec2(1.0f, 1.0f);
        glm::vec3 normal = mesh.HasNormals() ? glm::make_vec3(&mesh.mNormals[vertexIndex].x) : glm::vec3(0.0f, 1.0f, 0.0f);

        inOutVertexBuffer.Add(&pos, sizeof(glm::vec3));
        inOutVertexBuffer.Add(&uv, sizeof(glm::vec2));
        inOutVertexBuffer.Add(&normal, sizeof(glm::vec3));
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
        inOutVertexBuffer.Add(&tangent, sizeof(glm::vec3));
        inOutVertexBuffer.Add(&biTangent, sizeof(glm::vec3));
    }

    void ReadVertexData(const aiMesh& mesh, const FlagSet<AssimpMeshFactoryCreateFlags>& flags, const std::vector<VertexBoneData>& vertexBoneData, const unsigned int vertexBaseOffset, VertexDataBuffer& inOutVertexBuffer) const
    {
        for (unsigned int vertexIndex = 0; vertexIndex < mesh.mNumVertices; vertexIndex++) {
            AddDefaultVertexData(mesh, vertexIndex, inOutVertexBuffer);

            if (flags & AssimpMeshFactoryCreateFlags::ANIMATION) {
                AddAnimationData(vertexBoneData, vertexBaseOffset + vertexIndex, inOutVertexBuffer);
            }

            if (flags & AssimpMeshFactoryCreateFlags::BUMP_MAPPING) {
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

    unsigned int ReadMeshes(const aiScene& scene, const FlagSet<AssimpMeshFactoryCreateFlags>& flags, VertexDataBuffer& inOutVertexBuffer, std::vector<uint32_t>& inOutIndices) const
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
            ReadVertexData(assMesh, flags, vertexBoneData, vertexBaseOffset, inOutVertexBuffer);
            ReadIndices(assMesh, inOutIndices);
            vertexBaseOffset += assMesh.mNumVertices;
        }

        return allVertexCount;
    }
};

#endif