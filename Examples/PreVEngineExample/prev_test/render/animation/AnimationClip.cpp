#include "AnimationClip.h"

namespace prev_test::render::animation {
namespace {
    template <typename T>
    uint32_t FindKeyFrameIndexAfter(const std::vector<T>& keyFrames, const float animationTime)
    {
        for (uint32_t i = 0; i < keyFrames.size() - 1; ++i) {
            if (animationTime < keyFrames[i + 1].time) {
                return i;
            }
        }
        assert(0);
        return 0;
    }

    glm::vec3 Interpolate(const VectorKey& a, const VectorKey& b, const float animationTime)
    {
        const float deltaTime{ b.time - a.time };
        const float factor{ (animationTime - a.time) / deltaTime };
        assert(factor >= 0.0f && factor <= 1.0f);
        return a.value + factor * (b.value - a.value);
    }

    glm::quat Interpolate(const QuaternionKey& a, const QuaternionKey& b, const float animationTime)
    {
        const float deltaTime{ b.time - a.time };
        const float factor{ (animationTime - a.time) / deltaTime };
        assert(factor >= 0.0f && factor <= 1.0f);
        return glm::normalize(glm::slerp(a.value, b.value, factor));
    }
} // namespace

AnimationClip::AnimationClip(const glm::mat4& globaTransform, const AnimationNode& rootNode, const std::vector<AnimationNodeKeyFrames>& keyFrames, const std::vector<BoneInfo>& bones, float ticksPerSecond, float duration)
    : m_globaTransform{ globaTransform }
    , m_rootNode{ rootNode }
    , m_ticksPerSecond{ ticksPerSecond }
    , m_duration{ duration }
{
    // create effective bone mapping
    for (const auto& bone : bones) {
        if (m_boneMapping.find(bone.name) != m_boneMapping.cend()) {
            continue;
        }
        m_boneMapping[bone.name] = static_cast<uint32_t>(m_defaultBoneTransforms.size());
        m_defaultBoneTransforms.push_back(bone.transform);
    }

    // resize output bone transforms
    m_boneTransforms.resize(m_defaultBoneTransforms.size(), glm::mat4(1.0f));

    // convert keyFrames to map for faster lookup
    for (const auto& keyFrame : keyFrames) {
        m_keyFrames[keyFrame.name] = keyFrame;
    }
}

void AnimationClip::Update(const float deltaTime)
{
    const float scaledDeltaTime{ deltaTime * m_animationSpeed };

    if (m_animationState == prev_test::render::AnimationState::RUNNING) {
        m_elapsedTime += scaledDeltaTime;
        if (m_elapsedTime < 0.0f) {
            m_elapsedTime += m_duration;
        }
    } else if (m_animationState == prev_test::render::AnimationState::STOPPED) {
        m_elapsedTime = 0.0f;
    }

    const auto ticksPerSecond{ m_ticksPerSecond != 0 ? m_ticksPerSecond : 25.0f };
    const auto timeInTicks{ m_elapsedTime * ticksPerSecond };
    const auto animationTime{ fmod(timeInTicks, m_duration) };

    UpdateNodeHeirarchy(m_rootNode, m_globaTransform, animationTime);
}

const std::vector<glm::mat4>& AnimationClip::GetBoneTransforms() const
{
    return m_boneTransforms;
}

void AnimationClip::SetState(const prev_test::render::AnimationState state)
{
    m_animationState = state;
}

void AnimationClip::SetSpeed(const float speed)
{
    m_animationSpeed = speed;
}

void AnimationClip::SetTime(const float elapsed)
{
    m_elapsedTime = elapsed;
}

bool AnimationClip::FindAnimationNodeKeyFrames(const std::string& nodeName, AnimationNodeKeyFrames& outNodeKeyFrames) const
{
    auto keyFramesIter{ m_keyFrames.find(nodeName) };
    if (keyFramesIter == m_keyFrames.cend()) {
        return false;
    }
    outNodeKeyFrames = keyFramesIter->second;
    return true;
}

glm::vec3 AnimationClip::CalculateInterpolatedVector3(const std::vector<VectorKey>& vector3KeyFrames, const float animationTime) const
{
    glm::vec3 outVector3;
    if (vector3KeyFrames.size() == 1) {
        outVector3 = vector3KeyFrames[0].value;
    } else {
        const auto vector3Index{ FindKeyFrameIndexAfter(vector3KeyFrames, animationTime) };
        const auto nextVector3Index{ vector3Index + 1 };
        assert(nextVector3Index < static_cast<uint32_t>(vector3KeyFrames.size()));

        const auto& vector3{ vector3KeyFrames[vector3Index] };
        const auto& nextVector3{ vector3KeyFrames[nextVector3Index] };

        outVector3 = Interpolate(vector3, nextVector3, animationTime);
    }
    return outVector3;
}

glm::quat AnimationClip::CalculateInterpolatedQuaternion(const std::vector<QuaternionKey>& quaternionKeyFrames, const float animationTime) const
{
    glm::quat outQuaternion;
    if (quaternionKeyFrames.size() == 1) {
        outQuaternion = quaternionKeyFrames[0].value;
    } else {
        const auto quaternionIndex{ FindKeyFrameIndexAfter(quaternionKeyFrames, animationTime) };
        const auto nextQuaternionIndex{ quaternionIndex + 1 };
        assert(nextQuaternionIndex < static_cast<uint32_t>(quaternionKeyFrames.size()));

        const auto& quaternion{ quaternionKeyFrames[quaternionIndex] };
        const auto& nextQuaternion{ quaternionKeyFrames[nextQuaternionIndex] };

        outQuaternion = Interpolate(quaternion, nextQuaternion, animationTime);
    }
    return outQuaternion;
}

glm::mat4 AnimationClip::ComputeNodeTransform(const AnimationNode& node, const float animationTime) const
{
    auto nodeTransform{ node.transform };

    AnimationNodeKeyFrames nodeKeyFrames;
    if (FindAnimationNodeKeyFrames(node.name, nodeKeyFrames)) {
        const auto scaling{ CalculateInterpolatedVector3(nodeKeyFrames.scales, animationTime) };
        const auto scaleMatrix{ glm::scale(glm::mat4(1.0), glm::vec3(scaling.x, scaling.y, scaling.z)) };

        const auto rotationQuat{ CalculateInterpolatedQuaternion(nodeKeyFrames.rotations, animationTime) };
        const auto rotationMatrix{ glm::mat4_cast(rotationQuat) };

        const auto translation{ CalculateInterpolatedVector3(nodeKeyFrames.positions, animationTime) };
        const auto translationMatrix{ glm::translate(glm::mat4(1.0), glm::vec3(translation.x, translation.y, translation.z)) };

        nodeTransform = translationMatrix * rotationMatrix * scaleMatrix;
    }
    return nodeTransform;
}

void AnimationClip::UpdateNodeHeirarchy(const AnimationNode& node, const glm::mat4& parentTransform, const float animationTime)
{
    const auto nodeTransform{ ComputeNodeTransform(node, animationTime) };
    const auto globalTransform{ parentTransform * nodeTransform };

    if (m_boneMapping.find(node.name) != m_boneMapping.cend()) {
        const auto boneIndex{ m_boneMapping[node.name] };
        m_boneTransforms[boneIndex] = globalTransform * m_defaultBoneTransforms[boneIndex];
    }

    for (const auto& child : node.children) {
        UpdateNodeHeirarchy(child, globalTransform, animationTime);
    }
}
}