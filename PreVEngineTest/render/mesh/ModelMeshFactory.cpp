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
    mesh->m_verticesCount = ReadMeshes(*scene, flags, mesh->m_vertexDataBuffer, mesh->m_vertices, mesh->m_indices, mesh->m_meshParts);

    return mesh;
}

void ModelMeshFactory::ReadNodeHierarchy(const aiNode* node, const glm::mat4& parentTransform, std::vector<glm::mat4>& transforms) const
{
    const auto currentTransform = parentTransform * prev_test::render::util::assimp::AssimpGlmConvertor::ToGlmMat4(node->mTransformation);
    if (node->mNumMeshes > 0) {
        transforms.push_back(currentTransform);
    }

    for (uint32_t i = 0; i < node->mNumChildren; i++) {
        ReadNodeHierarchy(node->mChildren[i], currentTransform, transforms);
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

void ModelMeshFactory::ReadIndices(const aiMesh& mesh, std::vector<uint32_t>& inOutIndices) const
{
    for (unsigned int i = 0; i < mesh.mNumFaces; i++) {
        const aiFace& face = mesh.mFaces[i];
        for (unsigned int k = 0; k < face.mNumIndices; k++) {
            inOutIndices.emplace_back(face.mIndices[k]);
        }
    }
}

void ModelMeshFactory::AddDefaultVertexData(const aiMesh& mesh, const uint32_t vertexIndex, prev_test::render::VertexDataBuffer& inOutVertexBuffer) const
{
    glm::vec3 pos = glm::make_vec3(&mesh.mVertices[vertexIndex].x);
    glm::vec2 uv = mesh.mTextureCoords[0] != nullptr ? glm::make_vec3(&mesh.mTextureCoords[0][vertexIndex].x) : glm::vec2(1.0f, 1.0f);
    glm::vec3 normal = mesh.HasNormals() ? glm::make_vec3(&mesh.mNormals[vertexIndex].x) : glm::vec3(0.0f, 1.0f, 0.0f);

    inOutVertexBuffer.Add(pos);
    inOutVertexBuffer.Add(uv);
    inOutVertexBuffer.Add(normal);
}

void ModelMeshFactory::AddAnimationData(const std::vector<VertexBoneData>& vertexBoneData, const uint32_t vertexIndex, prev_test::render::VertexDataBuffer& inOutVertexBuffer) const
{
    const auto& singleVertexBoneData = vertexBoneData[vertexIndex];

    inOutVertexBuffer.Add(&singleVertexBoneData.ids, static_cast<uint32_t>(prev::util::ArraySize(singleVertexBoneData.ids) * sizeof(unsigned int)));
    inOutVertexBuffer.Add(&singleVertexBoneData.weights, static_cast<uint32_t>(prev::util::ArraySize(singleVertexBoneData.weights) * sizeof(float)));
}

void ModelMeshFactory::AddBumpMappingData(const aiMesh& mesh, const unsigned int vertexIndex, prev_test::render::VertexDataBuffer& inOutVertexBuffer) const
{
    glm::vec3 tangent = glm::make_vec3(&mesh.mTangents[vertexIndex].x);
    glm::vec3 biTangent = glm::make_vec3(&mesh.mBitangents[vertexIndex].x);
    inOutVertexBuffer.Add(tangent);
    inOutVertexBuffer.Add(biTangent);
}

void ModelMeshFactory::ReadVertexData(const aiMesh& mesh, const prev::common::FlagSet<CreateFlags>& flags, const std::vector<VertexBoneData>& vertexBoneData, const uint32_t vertexBaseOffset, prev_test::render::VertexDataBuffer& inOutVertexBuffer) const
{
    for (unsigned int vertexIndex = 0; vertexIndex < mesh.mNumVertices; vertexIndex++) {
        AddDefaultVertexData(mesh, vertexIndex, inOutVertexBuffer);

        if (flags & CreateFlags::ANIMATION) {
            AddAnimationData(vertexBoneData, vertexBaseOffset + vertexIndex, inOutVertexBuffer);
        }

        if (flags & CreateFlags::TANGENT_BITANGENT) {
            AddBumpMappingData(mesh, vertexIndex, inOutVertexBuffer);
        }
    }
}

std::vector<VertexBoneData> ModelMeshFactory::LoadAnimationBones(const aiMesh& mesh) const
{
    std::vector<VertexBoneData> bones(mesh.mNumVertices);
    std::map<std::string, uint32_t> boneMapping;

    for (uint32_t boneIndex = 0; boneIndex < mesh.mNumBones; boneIndex++) {
        const std::string currentBoneName{ mesh.mBones[boneIndex]->mName.data };

        uint32_t currentBoneIndex{ 0 };
        if (boneMapping.find(currentBoneName) == boneMapping.end()) {
            currentBoneIndex = static_cast<uint32_t>(boneMapping.size());
            boneMapping[currentBoneName] = currentBoneIndex;
        } else {
            currentBoneIndex = boneMapping[currentBoneName];
        }

        for (uint32_t j = 0; j < mesh.mBones[boneIndex]->mNumWeights; j++) {
            const uint32_t vertexId = mesh.mBones[boneIndex]->mWeights[j].mVertexId;
            const float weight = mesh.mBones[boneIndex]->mWeights[j].mWeight;
            bones[vertexId].AddBoneData(currentBoneIndex, weight);
        }
    }
    return bones;
}

unsigned int ModelMeshFactory::GetAllVertexCount(const aiScene& scene) const
{
    uint32_t vertexCount{ 0 };
    for (uint32_t meshIndex = 0; meshIndex < scene.mNumMeshes; meshIndex++) {
        vertexCount += scene.mMeshes[meshIndex]->mNumVertices;
    };
    return vertexCount;
}

unsigned int ModelMeshFactory::ReadMeshes(const aiScene& scene, const prev::common::FlagSet<CreateFlags>& flags, prev_test::render::VertexDataBuffer& inOutVertexBuffer, std::vector<glm::vec3>& inOutVertices, std::vector<uint32_t>& inOutIndices, std::vector<MeshPart>& inOutMeshParts) const
{
    uint32_t allVertexCount = GetAllVertexCount(scene);
    std::vector<VertexBoneData> vertexBoneData;
    if (flags & CreateFlags::ANIMATION) {
        vertexBoneData.resize(allVertexCount);

        uint32_t vertexBaseOffset = 0;
        for (uint32_t meshIndex = 0; meshIndex < scene.mNumMeshes; meshIndex++) {
            const aiMesh& assMesh = *scene.mMeshes[meshIndex];
            const auto vertexBonePart = LoadAnimationBones(assMesh);
            for (size_t j = 0; j < vertexBonePart.size(); j++) {
                vertexBoneData[j + vertexBaseOffset] = vertexBonePart[j];
            }
            vertexBaseOffset += assMesh.mNumVertices;
        }
    }

    std::vector<glm::mat4> transforms;
    ReadNodeHierarchy(scene.mRootNode, prev_test::render::util::assimp::AssimpGlmConvertor::ToGlmMat4(scene.mRootNode->mTransformation), transforms);

    uint32_t vertexBaseOffset = 0;
    uint32_t indexBaseOffset = 0;
    for (uint32_t meshIndex = 0; meshIndex < scene.mNumMeshes; meshIndex++) {
        const aiMesh& assMesh = *scene.mMeshes[meshIndex];

        const auto& currentTransform = transforms.at(meshIndex);
        for (uint32_t vi = 0; vi < assMesh.mNumVertices; vi++) {
            inOutVertices.push_back(currentTransform * glm::vec4(prev_test::render::util::assimp::AssimpGlmConvertor::ToGlmVec3(assMesh.mVertices[vi]), 1.0f));
        }

        ReadVertexData(assMesh, flags, vertexBoneData, vertexBaseOffset, inOutVertexBuffer);

        std::vector<uint32_t> meshIndices;
        ReadIndices(assMesh, meshIndices);

        inOutIndices.insert(inOutIndices.end(), meshIndices.begin(), meshIndices.end());
        inOutMeshParts.push_back(MeshPart{ vertexBaseOffset, indexBaseOffset, static_cast<uint32_t>(meshIndices.size()), transforms.at(meshIndex), assMesh.mMaterialIndex });

        vertexBaseOffset += assMesh.mNumVertices;
        indexBaseOffset += static_cast<uint32_t>(meshIndices.size());
    }

    return allVertexCount;
}
} // namespace prev_test::render::mesh