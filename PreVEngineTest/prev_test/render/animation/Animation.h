#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "../IAnimation.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <map>
#include <memory>

namespace prev_test::render::animation {
class AnimationFactory;

// TODO -> make AnimationClipImpl* member and hide there all the Assimp specific things
class AnimationClip : public prev_test::render::IAnimationClip {
public:
    AnimationClip(const aiScene* scene);

    ~AnimationClip() = default;

public:
    void Update(const float deltaTime) override;

    const std::vector<glm::mat4>& GetBoneTransforms() const override;

    void SetState(const prev_test::render::AnimationState state) override;

    void SetSpeed(const float speed) override;

    void SetTime(const float elapsed) override;

private:
    uint32_t FindPosition(const float animationTime, const aiNodeAnim* nodeAnimation) const;

    uint32_t FindRotation(const float animationTime, const aiNodeAnim* nodeAnimation) const;

    uint32_t FindScaling(const float animationTime, const aiNodeAnim* nodeAnimation) const;

    const aiNodeAnim* FindNodeAnimByName(const aiAnimation* animation, const std::string& nodeName) const;

    aiVector3D CalculateInterpolatedPosition(const float animationTime, const aiNodeAnim* nodeAnimation) const;

    aiQuaternion CalculateInterpolatedRotation(const float animationTime, const aiNodeAnim* nodeAnimation) const;

    aiVector3D CalculateInterpolatedScaling(const float animationTime, const aiNodeAnim* nodeAnimation) const;

    void UpdateNodeHeirarchy(const float animationTime, const aiNode* node, const glm::mat4& parentTransformation);

private:
    friend AnimationFactory;

private:
    struct BoneInfo {
        glm::mat4 boneOffset;

        glm::mat4 finalTransformation;
    };

private:
    const aiScene* m_scene{};

    float m_elapsedTime{ 0.0f };

    std::map<std::string, uint32_t> m_boneMapping; // maps a bone name to its index

    std::vector<BoneInfo> m_boneInfos;

    glm::mat4 m_globalInverseTransform{ 1.0f };

    std::vector<glm::mat4> m_boneTransforms;

    prev_test::render::AnimationState m_animationState{ prev_test::render::AnimationState::RUNNING };

    uint32_t m_index{ 0 };

    float m_animationSpeed{ 1.0f };
};

// TODO create AnimationImpl member and hide there all the Assimp specific things ???
class Animation : public prev_test::render::IAnimation {
public:
    Animation() = default;

    ~Animation() = default;

public:
    void Update(const float deltaTime) override;

    IAnimationClip& GetClip(const uint32_t clipIndex) const override;

    uint32_t GetClipCount() const override;

    void SetState(const AnimationState state) override;

    void SetSpeed(const float speed) override;

    void SetTime(const float elapsed) override;

private:
    friend AnimationFactory;

private:
    const aiScene* m_scene{};

    Assimp::Importer m_importer; // TODO get rid of that

    std::vector<std::shared_ptr<IAnimationClip>> m_clips;
};

} // namespace prev_test::render::animation

#endif // !__ANIMATION_H__
