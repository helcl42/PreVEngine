#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "../IAnimation.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <map>

namespace prev_test::render::animation {
class AnimationFactory;

class Animation : public prev_test::render::IAnimation {
public:
    Animation() = default;

    ~Animation() = default;

public:
    void Update(const float deltaTime) override;

    const std::vector<glm::mat4>& GetBoneTransforms() const override;

    void SetState(const prev_test::render::AnimationState animationState) override;

    prev_test::render::AnimationState GetState() const override;

    void SetIndex(const unsigned int index) override;

    unsigned int GetIndex() const override;

    void SetSpeed(const float speed) override;

    float GetSpeed() const override;

    void SetTime(const float elapsed) override;

    float GetTime() const override;

private:
    unsigned int FindPosition(const float animationTime, const aiNodeAnim* nodeAnimation) const;

    unsigned int FindRotation(const float animationTime, const aiNodeAnim* nodeAnimation) const;

    unsigned int FindScaling(const float animationTime, const aiNodeAnim* nodeAnimation) const;

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
    float m_elapsedTime{ 0.0f };

    std::map<std::string, unsigned int> m_boneMapping; // maps a bone name to its index

    unsigned int m_numBones{ 0 };

    std::vector<BoneInfo> m_boneInfos;

    glm::mat4 m_globalInverseTransform{ 1.0f };

    std::vector<glm::mat4> m_boneTransforms;

    const aiScene* m_scene{ nullptr };

    Assimp::Importer m_importer; // TODO get rid of that

    prev_test::render::AnimationState m_animationState{ prev_test::render::AnimationState::RUNNING };

    unsigned int m_animationIndex{ 0 };

    float m_animationSpeed{ 1.0f };
};
} // namespace prev_test::render::animation

#endif // !__ANIMATION_H__
