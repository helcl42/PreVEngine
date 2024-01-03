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

std::vector<glm::vec3> MeshUtil::GenerateNormals(const std::vector<glm::vec3>& vertices, const std::vector<uint32_t>& indices, const bool smooth)
{
    std::vector<glm::vec3> normals(vertices.size());
    for (size_t i = 0; i < indices.size() - 2; i += 3) {
        const auto index1{ indices[i + 0] };
        const auto index2{ indices[i + 1] };
        const auto index3{ indices[i + 2] };

        const auto& v1{ vertices[index1] };
        const auto& v2{ vertices[index2] };
        const auto& v3{ vertices[index3] };

        const auto deltaPos1{ v2 - v1 };
        const auto deltaPos2{ v3 - v1 };

        const auto normal{ glm::normalize(glm::cross(deltaPos1, deltaPos2)) };

        if (smooth) {
            normals[index1] += normal;
            normals[index2] += normal;
            normals[index3] += normal;
        } else {
            normals[index1] = normal;
            normals[index2] = normal;
            normals[index3] = normal;
        }
    }

    for (size_t i = 0; i < normals.size(); i++) {
        normals[i] = glm::normalize(normals[i]);
    }
    return normals;
}

std::tuple<std::vector<glm::vec3>, std::vector<glm::vec3>> MeshUtil::GenerateTangetsAndBiTangents(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& textureCoords, const std::vector<glm::vec3>& normals, const std::vector<uint32_t>& indices)
{
    std::vector<glm::vec3> tangents(vertices.size());
    std::vector<glm::vec3> biTangents(vertices.size());

    for (size_t i = 0; i < indices.size() - 2; i += 3) {
        const auto index1{ indices[i + 0] };
        const auto index2{ indices[i + 1] };
        const auto index3{ indices[i + 2] };

        const auto& v1{ vertices[index1] };
        const auto& v2{ vertices[index2] };
        const auto& v3{ vertices[index3] };

        const auto& uv1{ textureCoords[index1] };
        const auto& uv2{ textureCoords[index2] };
        const auto& uv3{ textureCoords[index3] };

        // Edges of the triangle : position delta
        const auto deltaPos1{ v2 - v1 };
        const auto deltaPos2{ v3 - v1 };

        // UV delta
        const auto deltaUV1{ uv2 - uv1 };
        const auto deltaUV2{ uv3 - uv1 };

        const float r{ 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x) };
        const glm::vec3 tangent{ glm::normalize((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r) };
        const glm::vec3 biTangent{ glm::normalize((deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r) };

        tangents[index1] += tangent;
        tangents[index2] += tangent;
        tangents[index3] += tangent;

        biTangents[index1] += biTangent;
        biTangents[index2] += biTangent;
        biTangents[index3] += biTangent;
    }

    for (size_t i = 0; i < vertices.size(); ++i) {
        const auto& n{ normals[i] };
        const auto& t{ tangents[i] };
        const auto& bt{ biTangents[i] };

        // Gram-Schmidt orthogonalize
        tangents[i] = glm::normalize(t - n * glm::dot(n, t));

        const float handeness{ glm::dot(glm::cross(n, tangents[i]), bt) < 0.0f ? -1.0f : 1.0f };
        biTangents[i] = glm::normalize(glm::cross(n, tangents[i]) * handeness);
    }

    return { tangents, biTangents };
}

std::vector<glm::vec3> MeshUtil::GetMeshTransformedVertices(const std::shared_ptr<prev_test::render::IMesh>& mesh)
{
    std::vector<glm::vec3> vertices;
    GetAllMeshVerticies(mesh, mesh->GetRootNode(), vertices);
    return vertices;
}
} // namespace prev_test::render::mesh