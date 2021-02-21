#include "ModelMeshFactory.h"
#include "ModelMesh.h"

#include "../util/assimp/AssimpGlmConvertor.h"
#include "../util/assimp/AssimpSceneLoader.h"

#include <prev/util/Utils.h>

#include <map>

namespace prev_test::render::mesh {
std::unique_ptr<prev_test::render::IMesh> ModelMeshFactory::Create(const std::string& modelPath, const prev::common::FlagSet<CreateFlags>& flags) const
{
    Assimp::Importer importer{};
    const aiScene* scene;

    prev_test::render::util::assimp::AssimpSceneLoader assimpSceneLoader{};
    if (!assimpSceneLoader.LoadScene(modelPath, &importer, &scene)) {
        throw std::runtime_error("Could not load model: " + modelPath);
    }

    auto mesh = std::make_unique<ModelMesh>();
    mesh->m_vertexLayout = GetVertexLayout(flags);
    mesh->m_verticesCount = GetSceneVertexCount(*scene);
    ReadNodeHierarchy(scene->mRootNode, prev_test::render::util::assimp::AssimpGlmConvertor::ToGlmMat4(scene->mRootNode->mTransformation), mesh->m_meshRootNode);
    ReadMeshes(*scene, flags, mesh->m_vertexDataBuffer, mesh->m_indices, mesh->m_meshParts);
    return mesh;
}

void ModelMeshFactory::ReadNodeHierarchy(const aiNode* node, const glm::mat4& parentTransform, MeshNode& meshNode) const
{
    const auto currentTransform{ parentTransform * prev_test::render::util::assimp::AssimpGlmConvertor::ToGlmMat4(node->mTransformation) };

    meshNode.transform = currentTransform;
    for (uint32_t i = 0; i < node->mNumMeshes; i++) {
        meshNode.meshPartIndices.push_back(node->mMeshes[i]);
    }

    for (uint32_t i = 0; i < node->mNumChildren; i++) {
        MeshNode childMeshNode{};
        ReadNodeHierarchy(node->mChildren[i], currentTransform, childMeshNode);
        meshNode.children.push_back(childMeshNode);
    }
}

prev_test::render::VertexLayout ModelMeshFactory::GetVertexLayout(const prev::common::FlagSet<CreateFlags>& flags) const
{
    if (flags & CreateFlags::ANIMATION && flags & CreateFlags::TANGENT_BITANGENT) {
        return { { prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC2, prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC4, prev_test::render::VertexLayoutComponent::VEC4, prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC3 } };
    } else if (flags & CreateFlags::ANIMATION) {
        return { { prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC2, prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC4, prev_test::render::VertexLayoutComponent::VEC4 } };
    } else if (flags & CreateFlags::TANGENT_BITANGENT) {
        return { { prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC2, prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC3 } };
    } else {
        return { { prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC2, prev_test::render::VertexLayoutComponent::VEC3 } };
    }
}

std::vector<uint32_t> ModelMeshFactory::ReadMeshIndices(const aiMesh& mesh) const
{
    std::vector<uint32_t> indices;
    for (uint32_t i = 0; i < mesh.mNumFaces; i++) {
        const aiFace& face = mesh.mFaces[i];
        for (uint32_t k = 0; k < face.mNumIndices; k++) {
            indices.emplace_back(face.mIndices[k]);
        }
    }
    return indices;
}

void ModelMeshFactory::AddDefaultVertexData(const aiMesh& mesh, const uint32_t vertexIndex, prev_test::render::VertexDataBuffer& inOutVertexBuffer, std::vector<glm::vec3>& inOutVertices) const
{
    const glm::vec3 pos{ glm::make_vec3(&mesh.mVertices[vertexIndex].x) };
    const glm::vec2 uv{ mesh.HasTextureCoords(0) ? glm::make_vec3(&mesh.mTextureCoords[0][vertexIndex].x) : glm::vec2(1.0f, 1.0f) };
    const glm::vec3 normal{ mesh.HasNormals() ? glm::make_vec3(&mesh.mNormals[vertexIndex].x) : glm::vec3(0.0f, 1.0f, 0.0f) };

    inOutVertexBuffer.Add(pos);
    inOutVertexBuffer.Add(uv);
    inOutVertexBuffer.Add(normal);

    inOutVertices.push_back(pos);
}

void ModelMeshFactory::AddAnimationData(const std::vector<VertexBoneData>& vertexBoneData, const uint32_t vertexIndex, prev_test::render::VertexDataBuffer& inOutVertexBuffer) const
{
    const auto& singleVertexBoneData{ vertexBoneData[vertexIndex] };

    inOutVertexBuffer.Add(&singleVertexBoneData.ids, static_cast<uint32_t>(prev::util::ArraySize(singleVertexBoneData.ids) * sizeof(unsigned int)));
    inOutVertexBuffer.Add(&singleVertexBoneData.weights, static_cast<uint32_t>(prev::util::ArraySize(singleVertexBoneData.weights) * sizeof(float)));
}

void ModelMeshFactory::AddTangentBitangentData(const aiMesh& mesh, const uint32_t vertexIndex, prev_test::render::VertexDataBuffer& inOutVertexBuffer) const
{
    const glm::vec3 tangent{ glm::make_vec3(&mesh.mTangents[vertexIndex].x) };
    const glm::vec3 biTangent{ glm::make_vec3(&mesh.mBitangents[vertexIndex].x) };

    inOutVertexBuffer.Add(tangent);
    inOutVertexBuffer.Add(biTangent);
}

void ModelMeshFactory::ReadMeshVertexData(const aiMesh& mesh, const prev::common::FlagSet<CreateFlags>& flags, const std::vector<VertexBoneData>& vertexBoneData, const uint32_t vertexBaseOffset, prev_test::render::VertexDataBuffer& inOutVertexBuffer, std::vector<glm::vec3>& inOutVertices) const
{
    for (uint32_t vertexIndex = 0; vertexIndex < mesh.mNumVertices; vertexIndex++) {
        AddDefaultVertexData(mesh, vertexIndex, inOutVertexBuffer, inOutVertices);

        if (flags & CreateFlags::ANIMATION) {
            AddAnimationData(vertexBoneData, vertexBaseOffset + vertexIndex, inOutVertexBuffer);
        }

        if (flags & CreateFlags::TANGENT_BITANGENT) {
            AddTangentBitangentData(mesh, vertexIndex, inOutVertexBuffer);
        }
    }
}

std::vector<VertexBoneData> ModelMeshFactory::ReadMeshBones(const aiMesh& mesh) const
{
    std::vector<VertexBoneData> vertexBoneData(mesh.mNumVertices);

    std::map<std::string, uint32_t> boneMapping;
    for (uint32_t boneIndex = 0; boneIndex < mesh.mNumBones; boneIndex++) {
        const auto& meshBone{ *mesh.mBones[boneIndex] };
        const std::string currentBoneName{ meshBone.mName.data };

        uint32_t currentBoneIndex{ 0 };
        if (boneMapping.find(currentBoneName) == boneMapping.end()) {
            currentBoneIndex = static_cast<uint32_t>(boneMapping.size());
            boneMapping[currentBoneName] = currentBoneIndex;
        } else {
            currentBoneIndex = boneMapping[currentBoneName];
        }

        for (uint32_t i = 0; i < meshBone.mNumWeights; i++) {
            const auto& vertexWeight{ meshBone.mWeights[i] };
            auto& singleVertexBoneData{ vertexBoneData[vertexWeight.mVertexId] };
            singleVertexBoneData.AddBoneData(currentBoneIndex, vertexWeight.mWeight);
        }
    }

    for (auto& bone : vertexBoneData) {
        bone.Normalize();
    }
    return vertexBoneData;
}

std::vector<VertexBoneData> ModelMeshFactory::ReadSceneBones(const aiScene& scene) const
{
    const auto allVertexCount{ GetSceneVertexCount(scene) };
    std::vector<VertexBoneData> vertexBoneData(allVertexCount);

    uint32_t vertexBaseOffset{ 0 };
    for (uint32_t meshIndex = 0; meshIndex < scene.mNumMeshes; meshIndex++) {
        const aiMesh& assMesh = *scene.mMeshes[meshIndex];
        const auto vertexBonePart = ReadMeshBones(assMesh);
        for (size_t j = 0; j < vertexBonePart.size(); j++) {
            vertexBoneData[j + vertexBaseOffset] = vertexBonePart[j];
        }
        vertexBaseOffset += assMesh.mNumVertices;
    }
    return vertexBoneData;
}

uint32_t ModelMeshFactory::GetSceneVertexCount(const aiScene& scene) const
{
    uint32_t vertexCount{ 0 };
    for (uint32_t meshIndex = 0; meshIndex < scene.mNumMeshes; meshIndex++) {
        vertexCount += scene.mMeshes[meshIndex]->mNumVertices;
    };
    return vertexCount;
}

void ModelMeshFactory::ReadMeshes(const aiScene& scene, const prev::common::FlagSet<CreateFlags>& flags, prev_test::render::VertexDataBuffer& inOutVertexBuffer, std::vector<uint32_t>& inOutIndices, std::vector<MeshPart>& inOutMeshParts) const
{
    std::vector<VertexBoneData> vertexBoneData;
    if (flags & CreateFlags::ANIMATION) {
        vertexBoneData = ReadSceneBones(scene);
    }

    uint32_t vertexBaseOffset{ 0 };
    uint32_t indexBaseOffset{ 0 };
    for (uint32_t meshIndex = 0; meshIndex < scene.mNumMeshes; meshIndex++) {
        const aiMesh& assMesh{ *scene.mMeshes[meshIndex] };

        std::vector<glm::vec3> vertices;
        ReadMeshVertexData(assMesh, flags, vertexBoneData, vertexBaseOffset, inOutVertexBuffer, vertices);

        const auto meshIndices{ ReadMeshIndices(assMesh) };
        inOutIndices.insert(inOutIndices.end(), meshIndices.begin(), meshIndices.end());

        inOutMeshParts.push_back(MeshPart{ vertexBaseOffset, indexBaseOffset, static_cast<uint32_t>(meshIndices.size()), vertices, assMesh.mMaterialIndex });

        vertexBaseOffset += assMesh.mNumVertices;
        indexBaseOffset += static_cast<uint32_t>(meshIndices.size());
    }
}
} // namespace prev_test::render::mesh