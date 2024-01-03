#include "Animation.h"

#include "../util/assimp/AssimpGlmConvertor.h"

namespace prev_test::render::animation {

/// ANIMATION PART

AnimationPart::AnimationPart(const aiScene* scene)
    : m_scene(scene)
{
}

void AnimationPart::Update(const float deltaTime)
{
    m_animationIndex %= m_scene->mNumAnimations;

    const float scaledDeltaTime = deltaTime * m_animationSpeed;
    const auto currentAnimation = m_scene->mAnimations[m_animationIndex];
    const float animationDuration = static_cast<float>(currentAnimation->mDuration);

    if (m_animationState == prev_test::render::AnimationState::RUNNING) {
        m_elapsedTime += scaledDeltaTime;
        if (m_elapsedTime < 0.0f) {
            m_elapsedTime += animationDuration;
        }
    } else if (m_animationState == prev_test::render::AnimationState::STOPPED) {
        m_elapsedTime = 0.0f;
    }

    const auto ticksPerSecond = static_cast<float>(currentAnimation->mTicksPerSecond != 0 ? currentAnimation->mTicksPerSecond : 25.0f);
    const auto timeInTicks = m_elapsedTime * ticksPerSecond;
    const auto animationTime = fmod(timeInTicks, animationDuration);

    UpdateNodeHeirarchy(animationTime, m_scene->mRootNode, glm::mat4(1.0f));

    m_boneTransforms.resize(m_boneInfos.size());
    for (size_t i = 0; i < m_boneInfos.size(); i++) {
        m_boneTransforms[i] = m_boneInfos[i].finalTransformation;
    }
}

const std::vector<glm::mat4>& AnimationPart::GetBoneTransforms() const
{
    return m_boneTransforms;
}

void AnimationPart::SetState(const prev_test::render::AnimationState state)
{
    m_animationState = state;
}

void AnimationPart::SetSpeed(const float speed)
{
    m_animationSpeed = speed;
}

void AnimationPart::SetTime(const float elapsed)
{
    m_elapsedTime = elapsed;
}

unsigned int AnimationPart::FindPosition(const float animationTime, const aiNodeAnim* nodeAnimation) const
{
    for (unsigned int i = 0; i < nodeAnimation->mNumPositionKeys - 1; i++) {
        if (animationTime < static_cast<float>(nodeAnimation->mPositionKeys[i + 1].mTime)) {
            return i;
        }
    }
    assert(0);
    return 0;
}

unsigned int AnimationPart::FindRotation(const float animationTime, const aiNodeAnim* nodeAnimation) const
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

unsigned int AnimationPart::FindScaling(const float animationTime, const aiNodeAnim* nodeAnimation) const
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

const aiNodeAnim* AnimationPart::FindNodeAnimByName(const aiAnimation* animation, const std::string& nodeName) const
{
    for (unsigned int i = 0; i < animation->mNumChannels; i++) {
        const auto nodeAnimation = animation->mChannels[i];
        if (std::string(nodeAnimation->mNodeName.data) == nodeName) {
            return nodeAnimation;
        }
    }
    return nullptr;
}

aiVector3D AnimationPart::CalculateInterpolatedPosition(const float animationTime, const aiNodeAnim* nodeAnimation) const
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

aiQuaternion AnimationPart::CalculateInterpolatedRotation(const float animationTime, const aiNodeAnim* nodeAnimation) const
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

aiVector3D AnimationPart::CalculateInterpolatedScaling(const float animationTime, const aiNodeAnim* nodeAnimation) const
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

void AnimationPart::UpdateNodeHeirarchy(const float animationTime, const aiNode* node, const glm::mat4& parentTransformation)
{
    const std::string nodeName{ node->mName.data };
    const auto currentAnimation = m_scene->mAnimations[m_animationIndex];
    auto nodeTransformation = prev_test::render::util::assimp::AssimpGlmConvertor::ToGlmMat4(node->mTransformation);

    const auto nodeAnimation = FindNodeAnimByName(currentAnimation, nodeName);
    if (nodeAnimation) {
        const auto scaling = CalculateInterpolatedScaling(animationTime, nodeAnimation);
        const auto scaleMatrix = glm::scale(glm::mat4(1.0), glm::vec3(scaling.x, scaling.y, scaling.z));

        const auto rotationQuat = CalculateInterpolatedRotation(animationTime, nodeAnimation);
        const auto rotationMatrix = prev_test::render::util::assimp::AssimpGlmConvertor::ToGlmMat4(rotationQuat.GetMatrix());

        const auto translation = CalculateInterpolatedPosition(animationTime, nodeAnimation);
        const auto translationMatrix = glm::translate(glm::mat4(1.0), glm::vec3(translation.x, translation.y, translation.z));

        nodeTransformation = translationMatrix * rotationMatrix * scaleMatrix;
    }

    const auto globalTransformation{ parentTransformation * nodeTransformation };
    if (m_boneMapping.find(nodeName) != m_boneMapping.cend()) {
        const auto boneIndex{ m_boneMapping[nodeName] };
        m_boneInfos[boneIndex].finalTransformation = m_globalInverseTransform * globalTransformation * m_boneInfos[boneIndex].boneOffset;
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        UpdateNodeHeirarchy(animationTime, node->mChildren[i], globalTransformation);
    }
}

/// ANIMATION

void Animation::Update(const float deltaTime)
{
    for (auto& part : m_parts) {
        part->Update(deltaTime);
    }
}

std::shared_ptr<IAnimationPart> Animation::GetAnimationPart(unsigned int partIndex) const
{
    return m_parts[partIndex];
}

const std::vector<std::shared_ptr<IAnimationPart>>& Animation::GetAnimationParts() const
{
    return m_parts;
}

void Animation::SetState(const AnimationState state)
{
    for (auto& part : m_parts) {
        part->SetState(state);
    }
}

void Animation::SetSpeed(const float speed)
{
    for (auto& part : m_parts) {
        part->SetSpeed(speed);
    }
}

void Animation::SetTime(const float elapsed)
{
    for (auto& part : m_parts) {
        part->SetTime(elapsed);
    }
}

} // namespace prev_test::render::animation