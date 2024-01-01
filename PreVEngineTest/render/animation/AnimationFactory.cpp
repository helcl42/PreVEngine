#include "AnimationFactory.h"
#include "Animation.h"

#include "../util/assimp/AssimpGlmConvertor.h"
#include "../util/assimp/AssimpSceneLoader.h"

#include <stdexcept>

namespace prev_test::render::animation {
std::unique_ptr<prev_test::render::IAnimation> AnimationFactory::Create(const std::string& modelPath, const unsigned int animationIndex) const
{
    // TODO copy asiimp animations to some local structure to avoid storing importer and whole scene !!!
    auto animation = std::make_unique<Animation>();

    prev_test::render::util::assimp::AssimpSceneLoader assimpSceneLoader{};
    if (!assimpSceneLoader.LoadScene(modelPath, &animation->m_importer, &animation->m_scene)) {
        throw std::runtime_error("Could not load model: " + modelPath);
    }

    for (unsigned int meshIndex = 0; meshIndex < animation->m_scene->mNumMeshes; meshIndex++) {
        const auto mesh = animation->m_scene->mMeshes[meshIndex];

        auto animationPart = std::make_shared<AnimationPart>(animation->m_scene);
        animationPart->m_globalInverseTransform = glm::inverse(prev_test::render::util::assimp::AssimpGlmConvertor::ToGlmMat4(animation->m_scene->mRootNode->mTransformation));
        animationPart->m_animationIndex = animationIndex;

        for (unsigned int i = 0; i < mesh->mNumBones; i++) {
            const auto bone = mesh->mBones[i];
            const std::string boneName{ bone->mName.data };
            if (animationPart->m_boneMapping.find(boneName) == animationPart->m_boneMapping.cend()) {
                animationPart->m_boneMapping[boneName] = static_cast<unsigned int>(animationPart->m_boneInfos.size());

                AnimationPart::BoneInfo boneInfo{};
                boneInfo.boneOffset = prev_test::render::util::assimp::AssimpGlmConvertor::ToGlmMat4(bone->mOffsetMatrix);
                animationPart->m_boneInfos.push_back(boneInfo);
            }
        }

        animation->m_parts.push_back(animationPart);
    }

    return animation;
}
} // namespace prev_test::render::animation