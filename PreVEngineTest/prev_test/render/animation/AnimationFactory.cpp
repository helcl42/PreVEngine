#include "AnimationFactory.h"
#include "Animation.h"

#include "../util/assimp/AssimpGlmConvertor.h"
#include "../util/assimp/AssimpSceneLoader.h"

#include <prev/common/Logger.h>

#include <stdexcept>

namespace prev_test::render::animation {
namespace {
    VectorKey CreateVectorKey(const aiVectorKey& aiVectorKey)
    {
        return { prev_test::render::util::assimp::AssimpGlmConvertor::ToGlmVec3(aiVectorKey.mValue), static_cast<float>(aiVectorKey.mTime) };
    }

    QuaternionKey CreateQuaternionKey(const aiQuatKey& aiQuaternionKey)
    {
        return { prev_test::render::util::assimp::AssimpGlmConvertor::ToGlmQuat(aiQuaternionKey.mValue), static_cast<float>(aiQuaternionKey.mTime) };
    }

    AnimationNodeKeyFrames ConvertToNodeToAnimationNode(const aiNodeAnim& aiNode)
    {
        AnimationNodeKeyFrames keyFrames{};
        keyFrames.name = std::string(aiNode.mNodeName.data);
        keyFrames.positions.resize(aiNode.mNumPositionKeys);
        for (uint32_t i = 0; i < aiNode.mNumPositionKeys; ++i) {
            keyFrames.positions[i] = CreateVectorKey(aiNode.mPositionKeys[i]);
        }
        keyFrames.rotations.resize(aiNode.mNumRotationKeys);
        for (uint32_t i = 0; i < aiNode.mNumRotationKeys; ++i) {
            keyFrames.rotations[i] = CreateQuaternionKey(aiNode.mRotationKeys[i]);
        }
        keyFrames.scales.resize(aiNode.mNumScalingKeys);
        for (uint32_t i = 0; i < aiNode.mNumScalingKeys; ++i) {
            keyFrames.scales[i] = CreateVectorKey(aiNode.mScalingKeys[i]);
        }
        return keyFrames;
    }

    void CreateNodeHierarchy(const aiNode& aiNode, AnimationNode& outNode)
    {
        outNode.name = std::string(aiNode.mName.data);
        outNode.transform = prev_test::render::util::assimp::AssimpGlmConvertor::ToGlmMat4(aiNode.mTransformation);
        outNode.children.resize(aiNode.mNumChildren);
        for (uint32_t i = 0; i < aiNode.mNumChildren; ++i) {
            CreateNodeHierarchy(*aiNode.mChildren[i], outNode.children[i]);
        }
    }

    std::unique_ptr<AnimationClip> CreateAnimationClip(const aiScene& scene, const aiAnimation& animation, const std::vector<BoneInfo>& bones)
    {
        const auto globalInverseTransform{ prev_test::render::util::assimp::AssimpGlmConvertor::ToGlmMat4(scene.mRootNode->mTransformation) };

        AnimationNode rootNode{};
        CreateNodeHierarchy(*scene.mRootNode, rootNode);

        std::vector<AnimationNodeKeyFrames> keyFrames;
        keyFrames.resize(animation.mNumChannels);
        for (uint32_t nodeIndex = 0; nodeIndex < animation.mNumChannels; ++nodeIndex) {
            const auto& animationNode{ *animation.mChannels[nodeIndex] };
            keyFrames[nodeIndex] = ConvertToNodeToAnimationNode(animationNode);
        }

        if (animation.mDuration == 0) {
            LOGW("Animation clip has duration of 0!");
        }

        const auto ticksPerSecond{ animation.mTicksPerSecond != 0 ? static_cast<float>(animation.mTicksPerSecond) : 25.0f };
        const auto duration{ static_cast<float>(animation.mDuration) };

        return std::make_unique<AnimationClip>(globalInverseTransform, rootNode, keyFrames, bones, ticksPerSecond, duration);
    }

    std::vector<std::unique_ptr<AnimationClip>> CreateAnimationClips(const aiScene& scene, const aiMesh& mesh)
    {
        std::vector<BoneInfo> bones;
        bones.resize(mesh.mNumBones);
        for (uint32_t i = 0; i < mesh.mNumBones; ++i) {
            const auto& bone{ *mesh.mBones[i] };
            bones[i] = BoneInfo{ bone.mName.data, prev_test::render::util::assimp::AssimpGlmConvertor::ToGlmMat4(bone.mOffsetMatrix) };
        }

        std::vector<std::unique_ptr<AnimationClip>> clips;
        for (uint32_t animationIndex = 0; animationIndex < scene.mNumAnimations; ++animationIndex) {
            const auto& animation{ *scene.mAnimations[animationIndex] };
            clips.emplace_back(CreateAnimationClip(scene, animation, bones));
        }
        return clips;
    }
} // namespace

std::unique_ptr<prev_test::render::IAnimation> AnimationFactory::Create(const std::string& modelPath) const
{
    const aiScene* scene{};
    Assimp::Importer importer{};

    prev_test::render::util::assimp::AssimpSceneLoader assimpSceneLoader{};
    if (!assimpSceneLoader.LoadScene(modelPath, importer, &scene)) {
        throw std::runtime_error("Could not load model: " + modelPath);
    }

    std::vector<std::unique_ptr<IAnimationClip>> clips{};
    for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
        const auto& mesh{ *scene->mMeshes[meshIndex] };
        auto meshClips{ CreateAnimationClips(*scene, mesh) };
        for (auto& meshClip : meshClips) {
            clips.emplace_back(std::move(meshClip));
        }
    }
    return std::make_unique<Animation>(std::move(clips));
}
} // namespace prev_test::render::animation