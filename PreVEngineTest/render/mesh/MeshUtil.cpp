#include "MeshUtil.h"

namespace prev_test::render::mesh {
namespace {
    void GetAllMeshVerticies(const std::shared_ptr<prev_test::render::IMesh>& mesh, const prev_test::render::MeshNode& parent, std::vector<glm::vec3>& inOutVertices)
    {
        const auto& meshParts{ mesh->GetMeshParts() };
        for (const auto idx : parent.meshPartIndices) {
            const auto& meshPart{ meshParts[idx] };
            for (const auto& v : meshPart.vertices) {
                inOutVertices.push_back(parent.transform * glm::vec4(v, 1.0f));
            }
        }

        for (const auto& ch : parent.children) {
            GetAllMeshVerticies(mesh, ch, inOutVertices);
        }
    }
} // namespace

void MeshUtil::GenerateTangetsAndBiTangents(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& textureCoords, const std::vector<uint32_t>& indices, std::vector<glm::vec3>& outTangents, std::vector<glm::vec3>& outBiTangents)
{
    outTangents.resize(vertices.size());
    outBiTangents.resize(vertices.size());

    for (size_t i = 0; i < indices.size() - 2; i += 3) {
        const auto indexA{ indices[i + 0] };
        const auto indexB{ indices[i + 1] };
        const auto indexC{ indices[i + 2] };

        const auto& v0{ vertices[indexA] };
        const auto& v1{ vertices[indexB] };
        const auto& v2{ vertices[indexC] };

        const auto& uv0{ textureCoords[indexA] };
        const auto& uv1{ textureCoords[indexB] };
        const auto& uv2{ textureCoords[indexC] };

        // Edges of the triangle : position delta
        const auto deltaPos1{ v1 - v0 };
        const auto deltaPos2{ v2 - v0 };

        // UV delta
        const auto deltaUV1{ uv1 - uv0 };
        const auto deltaUV2{ uv2 - uv0 };

        const float r{ 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x) };
        const glm::vec3 tangent{ (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r };
        const glm::vec3 biTangent{ (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r };

        outTangents[indexA] += tangent;
        outBiTangents[indexA] += biTangent;
        outTangents[indexB] += tangent;
        outBiTangents[indexB] += biTangent;
        outTangents[indexC] += tangent;
        outBiTangents[indexC] += biTangent;
    }
}

std::vector<glm::vec3> MeshUtil::GetMeshTransformedVertices(const std::shared_ptr<prev_test::render::IMesh>& mesh)
{
    std::vector<glm::vec3> vertices;
    GetAllMeshVerticies(mesh, mesh->GetRootNode(), vertices);
    return vertices;
}
} // namespace prev_test::render::mesh