#include "AnimationFactory.h"
#include "Animation.h"

#include "../util/assimp/AssimpGlmConvertor.h"
#include "../util/assimp/AssimpSceneLoader.h"

namespace prev_test::render::animation {
std::unique_ptr<prev_test::render::IAnimation> AnimationFactory::Create(const std::string& modelPath) const
{
    // TODO copy asiimp animations to some local structure to avoid storing importer and whole scene !!!
    auto animation = std::make_unique<Animation>();

    prev_test::render::util::assimp::AssimpSceneLoader assimpSceneLoader{};
    if (!assimpSceneLoader.LoadScene(modelPath, &animation->m_importer, &animation->m_scene)) {
        throw std::runtime_error("Could not load model: " + modelPath);
    }

    animation->m_globalInverseTransform = glm::inverse(prev_test::render::util::assimp::AssimpGlmConvertor::ToGlmMat4(animation->m_scene->mRootNode->mTransformation));

    for (unsigned int meshIndex = 0; meshIndex < animation->m_scene->mNumMeshes; meshIndex++) {
        const auto& mesh = *animation->m_scene->mMeshes[meshIndex];
        for (unsigned int i = 0; i < mesh.mNumBones; i++) {
            const auto bone = mesh.mBones[i];
            const std::string boneName{ bone->mName.data };
            if (animation->m_boneMapping.find(boneName) == animation->m_boneMapping.end()) {

                animation->m_boneMapping[boneName] = static_cast<unsigned int>(animation->m_boneInfos.size());

                auto boneInfo = Animation::BoneInfo{};
                boneInfo.boneOffset = prev_test::render::util::assimp::AssimpGlmConvertor::ToGlmMat4(mesh.mBones[i]->mOffsetMatrix);
                animation->m_boneInfos.push_back(boneInfo);
            }
        }
    }

    return animation;
}
} // namespace prev_test::render::animation