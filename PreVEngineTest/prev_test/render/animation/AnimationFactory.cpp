#include "AnimationFactory.h"
#include "Animation.h"

#include "../util/assimp/AssimpGlmConvertor.h"
#include "../util/assimp/AssimpSceneLoader.h"

#include <stdexcept>

namespace prev_test::render::animation {
std::unique_ptr<prev_test::render::IAnimation> AnimationFactory::Create(const std::string& modelPath, const uint32_t clipIndex) const
{
    // TODO copy asiimp animations to some local structure to avoid storing importer and whole scene !!!
    auto animation = std::make_unique<Animation>();

    prev_test::render::util::assimp::AssimpSceneLoader assimpSceneLoader{};
    if (!assimpSceneLoader.LoadScene(modelPath, animation->m_importer, &animation->m_scene)) {
        throw std::runtime_error("Could not load model: " + modelPath);
    }

    for (uint32_t meshIndex = 0; meshIndex < animation->m_scene->mNumMeshes; ++meshIndex) {
        const auto mesh = animation->m_scene->mMeshes[meshIndex];

        auto animationClip = std::make_shared<AnimationClip>(animation->m_scene);
        animationClip->m_globalInverseTransform = glm::inverse(prev_test::render::util::assimp::AssimpGlmConvertor::ToGlmMat4(animation->m_scene->mRootNode->mTransformation));
        animationClip->m_index = clipIndex;

        for (uint32_t i = 0; i < mesh->mNumBones; ++i) {
            const auto bone = mesh->mBones[i];
            const std::string boneName{ bone->mName.data };
            if (animationClip->m_boneMapping.find(boneName) == animationClip->m_boneMapping.cend()) {
                animationClip->m_boneMapping[boneName] = static_cast<uint32_t>(animationClip->m_boneInfos.size());

                AnimationClip::BoneInfo boneInfo{};
                boneInfo.boneOffset = prev_test::render::util::assimp::AssimpGlmConvertor::ToGlmMat4(bone->mOffsetMatrix);
                animationClip->m_boneInfos.push_back(boneInfo);
            }
        }

        animation->m_clips.push_back(animationClip);
    }

    return animation;
}
} // namespace prev_test::render::animation