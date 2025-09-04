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

glm::vec3 AnimationClip::CalculateInterpolatedPosition(const AnimationNodeKeyFrames& keyFrames, const float animationTime) const
{
    glm::vec3 outPosition;
    if (keyFrames.positions.size() == 1) {
        outPosition = keyFrames.positions[0].value;
    } else {
        const auto positionIndex{ FindKeyFrameIndexAfter(keyFrames.positions, animationTime) };
        const auto nextPositionIndex{ positionIndex + 1 };
        assert(nextPositionIndex < static_cast<uint32_t>(keyFrames.positions.size()));

        const auto& position{ keyFrames.positions[positionIndex] };
        const auto& nextPosition{ keyFrames.positions[nextPositionIndex] };

        outPosition = Interpolate(position, nextPosition, animationTime);
    }
    return outPosition;
}

glm::quat AnimationClip::CalculateInterpolatedRotation(const AnimationNodeKeyFrames& keyFrames, const float animationTime) const
{
    glm::quat outRotation;
    if (keyFrames.rotations.size() == 1) {
        outRotation = keyFrames.rotations[0].value;
    } else {
        const auto rotationIndex{ FindKeyFrameIndexAfter(keyFrames.rotations, animationTime) };
        const auto nextRotationIndex{ rotationIndex + 1 };
        assert(nextRotationIndex < static_cast<uint32_t>(keyFrames.rotations.size()));

        const auto rotation{ keyFrames.rotations[rotationIndex] };
        const auto nextRotation{ keyFrames.rotations[nextRotationIndex] };

        outRotation = Interpolate(rotation, nextRotation, animationTime);
    }
    return outRotation;
}

glm::vec3 AnimationClip::CalculateInterpolatedScaling(const AnimationNodeKeyFrames& keyFrames, const float animationTime) const
{
    glm::vec3 outScaling;
    if (keyFrames.scales.size() == 1) {
        outScaling = keyFrames.scales[0].value;
    } else {
        const auto scalingIndex{ FindKeyFrameIndexAfter(keyFrames.scales, animationTime) };
        const auto nextScalingIndex{ scalingIndex + 1 };
        assert(nextScalingIndex < static_cast<uint32_t>(keyFrames.scales.size()));

        const auto scaling{ keyFrames.scales[scalingIndex] };
        const auto nextScaling{ keyFrames.scales[nextScalingIndex] };

        outScaling = Interpolate(scaling, nextScaling, animationTime);
    }
    return outScaling;
}

glm::mat4 AnimationClip::ComputeNodeTransform(const AnimationNode& node, const float animationTime) const
{
    auto nodeTransform{ node.transform };

    AnimationNodeKeyFrames nodeKeyFrames;
    if (FindAnimationNodeKeyFrames(node.name, nodeKeyFrames)) {
        const auto scaling{ CalculateInterpolatedScaling(nodeKeyFrames, animationTime) };
        const auto scaleMatrix{ glm::scale(glm::mat4(1.0), glm::vec3(scaling.x, scaling.y, scaling.z)) };

        const auto rotationQuat{ CalculateInterpolatedRotation(nodeKeyFrames, animationTime) };
        const auto rotationMatrix{ glm::mat4_cast(rotationQuat) };

        const auto translation{ CalculateInterpolatedPosition(nodeKeyFrames, animationTime) };
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