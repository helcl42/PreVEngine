#ifndef __MODEL_MESH_FACTORY_H__
#define __MODEL_MESH_FACTORY_H__

#include "../IMesh.h"
#include "../VertexDataBuffer.h"
#include "VertexBoneData.h"

#include <prev/common/FlagSet.h>

#include <assimp/scene.h>

#include <memory>

namespace prev_test::render::mesh {
class ModelMeshFactory final {
public:
    enum class CreateFlags {
        ANIMATION,
        TANGENT_BITANGENT,
        _
    };

public:
    std::unique_ptr<prev_test::render::IMesh> Create(const std::string& modelPath, const prev::common::FlagSet<CreateFlags>& flags = prev::common::FlagSet<ModelMeshFactory::CreateFlags>{}) const;

private:
    void ReadNodeHierarchy(const aiNode* node, const glm::mat4& parentTransform, MeshNode& intOutRootMeshNode) const;

    prev_test::render::VertexLayout GetVertexLayout(const prev::common::FlagSet<CreateFlags>& flags) const;

    std::vector<uint32_t> ReadMeshIndices(const aiMesh& mesh) const;

    void AddDefaultVertexData(const aiMesh& mesh, const uint32_t vertexIndex, prev_test::render::VertexDataBuffer& inOutVertexBuffer, std::vector<glm::vec3>& inOutVertices) const;

    void AddAnimationData(const std::vector<VertexBoneData>& vertexBoneData, const uint32_t vertexIndex, prev_test::render::VertexDataBuffer& inOutVertexBuffer) const;

    void AddTangentBitangentData(const aiMesh& mesh, const uint32_t vertexIndex, prev_test::render::VertexDataBuffer& inOutVertexBuffer) const;

    void ReadMeshVertexData(const aiMesh& mesh, const prev::common::FlagSet<CreateFlags>& flags, const std::vector<VertexBoneData>& vertexBoneData, const uint32_t vertexBaseOffset, prev_test::render::VertexDataBuffer& inOutVertexBuffer, std::vector<glm::vec3>& inOutVertices) const;

    std::vector<VertexBoneData> ReadMeshBones(const aiMesh& mesh) const;
    
    std::vector<VertexBoneData> ReadSceneBones(const aiScene& scene) const;

    uint32_t GetSceneVertexCount(const aiScene& scene) const;

    void ReadMeshes(const aiScene& scene, const prev::common::FlagSet<CreateFlags>& flags, prev_test::render::VertexDataBuffer& inOutVertexBuffer, std::vector<glm::vec3>& inOutVertices, std::vector<uint32_t>& inOutIndices, std::vector<MeshPart>& inOutMeshParts) const;
};

} // namespace prev_test::render::mesh

#endif // !__MODEL_FACTORY_H__
