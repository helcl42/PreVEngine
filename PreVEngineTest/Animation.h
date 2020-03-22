#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "General.h"
#include "AssimpCommon.h"

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

#endif // !__ANIMATION_H__
