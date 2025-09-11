#ifndef __ANIMATION_CLIP_H__
#define __ANIMATION_CLIP_H__

#include "../IAnimation.h"

#include <map>
#include <vector>

namespace prev_test::render::animation {
struct VectorKey {
    glm::vec3 value{};
    float time{};
};

struct QuaternionKey {
    glm::quat value{};
    float time{};
};

struct BoneInfo {
    std::string name{};
    glm::mat4 transform{ 1.0f };
};

struct AnimationNode {
    std::string name{};
    glm::mat4 transform{ 1.0f };
    std::vector<AnimationNode> children;
};

struct AnimationNodeKeyFrames {
    std::string name{};
    std::vector<VectorKey> positions;
    std::vector<QuaternionKey> rotations;
    std::vector<VectorKey> scales;
};

class AnimationClip : public prev_test::render::IAnimationClip {
public:
    AnimationClip(const glm::mat4& globaTransform, const AnimationNode& rootNode, const std::vector<AnimationNodeKeyFrames>& keyFrames, const std::vector<BoneInfo>& bones, float ticksPerSecond, float duration);

    ~AnimationClip() = default;

public:
    void Update(const float deltaTime) override;

    const std::vector<glm::mat4>& GetBoneTransforms() const override;

    void SetState(const prev_test::render::AnimationState state) override;

    void SetSpeed(const float speed) override;

    void SetTime(const float elapsed) override;

private:
    bool FindAnimationNodeKeyFrames(const std::string& nodeName, AnimationNodeKeyFrames& outNodeKeyFrames) const;

    glm::vec3 CalculateInterpolatedVector3(const std::vector<VectorKey>& vector3KeyFrames, const float animationTime) const;

    glm::quat CalculateInterpolatedQuaternion(const std::vector<QuaternionKey>& quaternionKeyFrames, const float animationTime) const;

    glm::mat4 ComputeNodeTransform(const AnimationNode& node, const float animationTime) const;

    void UpdateNodeHeirarchy(const AnimationNode& node, const glm::mat4& parentTransforma, const float animationTime);

private:
    glm::mat4 m_globaTransform;

    AnimationNode m_rootNode;

    float m_ticksPerSecond;

    float m_duration;

    std::map<std::string, AnimationNodeKeyFrames> m_keyFrames;

    std::vector<glm::mat4> m_defaultBoneTransforms;

    std::map<std::string, uint32_t> m_boneMapping;

    float m_elapsedTime{ 0.0f };

    std::vector<glm::mat4> m_boneTransforms;

    prev_test::render::AnimationState m_animationState{ prev_test::render::AnimationState::RUNNING };

    float m_animationSpeed{ 1.0f };
};

} // namespace prev_test::render::animation

#endif