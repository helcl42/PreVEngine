#include "MeshFactory.h"

#include "Mesh.h"
#include "MeshUtil.h"

namespace prev_test::render::mesh {
namespace {
    template <typename... Args>
    void AddVertexData(VertexDataBuffer& buffer, Args&&... args)
    {
        (buffer.Add(std::forward<Args>(args)), ...);
    }

    std::vector<glm::vec3> ConvertToVec3(const std::vector<glm::vec2>& vertices)
    {
        std::vector<glm::vec3> outVertices(vertices.size());
        for (size_t i = 0; i < vertices.size(); ++i) {
            const auto& vertex2d{ vertices[i] };
            outVertices[i] = { vertex2d.x, vertex2d.y, 0.0f };
        }
        return outVertices;
    }

    glm::vec3 CreateConstellatedVertex(const FlatMeshConstellation constellation, const float x, const float z)
    {
        switch (constellation) {
        case FlatMeshConstellation::ZERO_X:
            return { 0.0f, x, z };
        case FlatMeshConstellation::ZERO_Z:
            return { x, z, 0.0f };
        default:
            return { x, 0.0f, z };
        }
    };

    glm::vec3 CreateConstellatedNormal(const FlatMeshConstellation constellation)
    {
        switch (constellation) {
        case FlatMeshConstellation::ZERO_X:
            return { 1.0f, 0.0f, 0.0f };
        case FlatMeshConstellation::ZERO_Z:
            return { 0.0f, 0.0f, 1.0f };
        default:
            return { 0.0f, 1.0f, 0.0f };
        }
    };
} // namespace

std::unique_ptr<prev_test::render::IMesh> MeshFactory::CreatePlane(const float xSize, const float zSize, const uint32_t xDivs, const uint32_t zDivs, const float textureCoordUMax, const float textureCoordVMax, const FlatMeshConstellation constellation, const bool generateTangentBiTangent) const
{
    const float x2{ xSize / 2.0f };
    const float z2{ zSize / 2.0f };
    const float iFactor{ zSize / static_cast<float>(zDivs) };
    const float jFactor{ xSize / static_cast<float>(xDivs) };
    const float texi{ textureCoordUMax / static_cast<float>(zDivs) };
    const float texj{ textureCoordVMax / static_cast<float>(xDivs) };

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> textureCoords;
    std::vector<glm::vec3> normals;

    for (uint32_t i = 0; i <= zDivs; ++i) {
        const float z{ iFactor * i - z2 };
        for (uint32_t j = 0; j <= xDivs; ++j) {
            const float x{ jFactor * j - x2 };

            const glm::vec3 vertex{ CreateConstellatedVertex(constellation, x, z) };
            const glm::vec2 textureCoord{ j * texj, i * texi };
            const glm::vec3 normal{ CreateConstellatedNormal(constellation) };

            vertices.push_back(vertex);
            textureCoords.push_back(textureCoord);
            normals.push_back(normal);
        }
    }

    std::vector<uint32_t> indices;
    for (uint32_t i = 0; i < zDivs; ++i) {
        const uint32_t rowStart{ i * (xDivs + 1) };
        const uint32_t nextRowStart{ (i + 1) * (xDivs + 1) };
        for (uint32_t j = 0; j < xDivs; j++) {
            const uint32_t ids[] = {
                rowStart + j,
                rowStart + j + 1,
                nextRowStart + j + 1,

                nextRowStart + j + 1,
                nextRowStart + j,
                rowStart + j,
            };

            for (const auto index : ids) {
                indices.emplace_back(index);
            }
        }
    }

    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> biTangents;
    if (generateTangentBiTangent) {
        std::tie(tangents, biTangents) = prev_test::render::mesh::MeshUtil::GenerateTangetsAndBiTangents(vertices, textureCoords, normals, indices);
    }

    if (generateTangentBiTangent) {
        return CreateFromData(vertices, textureCoords, normals, tangents, biTangents, indices);
    } else {
        return CreateFromData(vertices, textureCoords, normals, indices);
    }
}

std::unique_ptr<prev_test::render::IMesh> MeshFactory::CreateCube(const bool generateTangentBiTangent) const
{
    const std::vector<glm::vec3> vertices = {
        // FROMT
        { -0.5f, 0.5f, 0.5f },
        { -0.5f, -0.5f, 0.5f },
        { 0.5f, -0.5f, 0.5f },
        { 0.5f, 0.5f, 0.5f },
        // BACK
        { -0.5f, 0.5f, -0.5f },
        { 0.5f, 0.5f, -0.5f },
        { 0.5f, -0.5f, -0.5f },
        { -0.5f, -0.5f, -0.5f },
        // TOP
        { -0.5f, 0.5f, 0.5f },
        { 0.5f, 0.5f, 0.5f },
        { 0.5f, 0.5f, -0.5f },
        { -0.5f, 0.5f, -0.5f },
        // BOTTOM
        { -0.5f, -0.5f, 0.5f },
        { -0.5f, -0.5f, -0.5f },
        { 0.5f, -0.5f, -0.5f },
        { 0.5f, -0.5f, 0.5f },
        // LEFT
        { -0.5f, -0.5f, 0.5f },
        { -0.5f, 0.5f, 0.5f },
        { -0.5f, 0.5f, -0.5f },
        { -0.5f, -0.5f, -0.5f },
        // RIGHT
        { 0.5f, -0.5f, 0.5f },
        { 0.5f, -0.5f, -0.5f },
        { 0.5f, 0.5f, -0.5f },
        { 0.5f, 0.5f, 0.5f }
    };

    const std::vector<glm::vec2> textureCoords = {
        // FROMT
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        { 1.0f, 0.0f },
        { 0.0f, 0.0f },
        // BACK
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        { 1.0f, 0.0f },
        { 0.0f, 0.0f },
        // TOP
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        { 1.0f, 0.0f },
        { 0.0f, 0.0f },
        // BOTTOM
        { 0.0f, 1.0f },
        { 0.0f, 0.0f },
        { 1.0f, 0.0f },
        { 1.0f, 1.0f },
        // LEFT
        { 0.0f, 1.0f },
        { 0.0f, 0.0f },
        { 1.0f, 0.0f },
        { 1.0f, 1.0f },
        // RIGHT
        { 0.0f, 1.0f },
        { 0.0f, 0.0f },
        { 1.0f, 0.0f },
        { 1.0f, 1.0f },
    };

    const std::vector<uint32_t> indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };

    const auto normals{ prev_test::render::mesh::MeshUtil::GenerateNormals(vertices, indices, false) };

    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> biTangents;
    if (generateTangentBiTangent) {
        std::tie(tangents, biTangents) = prev_test::render::mesh::MeshUtil::GenerateTangetsAndBiTangents(vertices, textureCoords, normals, indices);
    }

    if (generateTangentBiTangent) {
        return CreateFromData(vertices, textureCoords, normals, tangents, biTangents, indices);
    } else {
        return CreateFromData(vertices, textureCoords, normals, indices);
    }
}

std::unique_ptr<prev_test::render::IMesh> MeshFactory::CreateSphere(const float radius, const int subDivY, const int subDivZ, const float degreesHorizontal, const float degreesVertical, const glm::vec3& position, const bool generateTangentBiTangent) const
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> textureCoords;
    std::vector<glm::vec3> normals;

    std::vector<uint32_t> indices;

    uint32_t indexBase{ 0 };

    const float addAngleY{ -degreesHorizontal / static_cast<float>(subDivY) };
    const float addAngleZ{ degreesVertical / static_cast<float>(subDivZ) };

    float curAngleY{ 0.0f };
    int stepsY{ 1 };
    while (stepsY <= subDivY) {
        const float sinY = sinf(glm::radians(curAngleY));
        const float cosY = cosf(glm::radians(curAngleY));
        const glm::vec3 directionY(cosY, 0.0f, -sinY);

        const float nextAngleY = curAngleY + addAngleY;
        const float nextSinY = sinf(glm::radians(nextAngleY));
        const float nextCosY = cosf(glm::radians(nextAngleY));
        const glm::vec3 nextDirectionY(nextCosY, 0.0f, -nextSinY);

        float currentAngleZ{ 0.0f };
        int stepsZ{ 1 };
        while (stepsZ <= subDivZ) {
            const float sinZ = sinf(glm::radians(currentAngleZ));
            const float cosZ = cosf(glm::radians(currentAngleZ));

            const float nextAngleZ = currentAngleZ + addAngleZ;
            const float nextSinZ = sinf(glm::radians(nextAngleZ));
            const float nextCosZ = cosf(glm::radians(nextAngleZ));

            const glm::vec3 quadPoints[] = {
                glm::vec3{ directionY.x * sinZ * radius, cosZ * radius, directionY.z * sinZ * radius } + position,
                glm::vec3{ directionY.x * nextSinZ * radius, nextCosZ * radius, directionY.z * nextSinZ * radius } + position,
                glm::vec3{ nextDirectionY.x * nextSinZ * radius, nextCosZ * radius, nextDirectionY.z * nextSinZ * radius } + position,
                glm::vec3{ nextDirectionY.x * sinZ * radius, cosZ * radius, nextDirectionY.z * sinZ * radius } + position,
            };

            const glm::vec3 ns[] = {
                glm::normalize(quadPoints[0]),
                glm::normalize(quadPoints[1]),
                glm::normalize(quadPoints[2]),
                glm::normalize(quadPoints[3]),
            };

            glm::vec2 tcs[4];
            float offset = 0.75f;
            float tx1 = 0.0f;
            float ty1 = 0.0f;
            for (unsigned int i = 0; i < 4; ++i) {
                float tx = atan2f(ns[i].x, ns[i].z) / glm::radians(fabsf(degreesHorizontal)) + 0.5f;
                float ty = asinf(ns[i].y) / glm::radians(fabsf(degreesVertical)) + 0.5f;
                if (i > 0) {
                    if (tx < offset && tx1 > offset) {
                        tx += 1.0f;
                    } else if (tx > offset && tx1 < offset) {
                        tx -= 1.0f;
                    }
                } else {
                    tx1 = tx;
                    ty1 = ty;
                }
                tcs[i] = glm::vec2(tx, ty);
            }

            for (auto i = 0; i < 4; ++i) {
                vertices.push_back(quadPoints[i]);
                textureCoords.push_back(tcs[i]);
                normals.push_back(ns[i]);
            }

            const uint32_t quadIndices[] = { 0, 1, 2, 2, 3, 0 };
            for (const auto idx : quadIndices) {
                indices.push_back(indexBase + idx);
            }

            indexBase += 4;

            stepsZ++;
            currentAngleZ += addAngleZ;
        }
        stepsY++;
        curAngleY += addAngleY;
    }

    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> biTangents;
    if (generateTangentBiTangent) {
        std::tie(tangents, biTangents) = prev_test::render::mesh::MeshUtil::GenerateTangetsAndBiTangents(vertices, textureCoords, normals, indices);
    }

    if (generateTangentBiTangent) {
        return CreateFromData(vertices, textureCoords, normals, tangents, biTangents, indices);
    } else {
        return CreateFromData(vertices, textureCoords, normals, indices);
    }
}

std::unique_ptr<prev_test::render::IMesh> MeshFactory::CreateQuad(const FlatMeshConstellation constellation, const bool generateTangentBiTangent) const
{
    const std::vector<glm::vec2> vertices2D = {
        { 1.0f, 1.0f },
        { -1.0f, 1.0f },
        { -1.0f, -1.0f },
        { 1.0f, -1.0f }
    };

    std::vector<glm::vec3> vertices;
    for (const auto& v : vertices2D) {
        vertices.push_back(CreateConstellatedVertex(constellation, v.x, v.y));
    }

    const std::vector<glm::vec2> textureCoords = {
        { 1.0f, 1.0f },
        { 0.0f, 1.0f },
        { 0.0f, 0.0f },
        { 1.0f, 0.0f }
    };

    const std::vector<uint32_t> indices = {
        0, 2, 1, 2, 0, 3
    };

    const auto normals{ prev_test::render::mesh::MeshUtil::GenerateNormals(vertices, indices, false) };

    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> biTangents;
    if (generateTangentBiTangent) {
        std::tie(tangents, biTangents) = prev_test::render::mesh::MeshUtil::GenerateTangetsAndBiTangents(vertices, textureCoords, normals, indices);
    }

    if (generateTangentBiTangent) {
        return CreateFromData(vertices, textureCoords, normals, tangents, biTangents, indices);
    } else {
        return CreateFromData(vertices, textureCoords, normals, indices);
    }
}

std::unique_ptr<prev_test::render::IMesh> MeshFactory::CreateFromData(const std::vector<glm::vec2>& vertices, const std::vector<uint32_t>& indices) const
{
    const prev_test::render::VertexLayout vertexLayout{ { prev_test::render::VertexLayoutComponent::VEC2 } };

    prev_test::render::VertexDataBuffer vertexDataBuffer;
    for (size_t i = 0; i < vertices.size(); ++i) {
        AddVertexData(vertexDataBuffer, vertices[i]);
    }

    const MeshNode rootNode{ { 0 }, glm::mat4{ 1.0f }, {} };
    const std::vector<prev_test::render::MeshPart> meshParts = { { static_cast<uint32_t>(indices.size()), ConvertToVec3(vertices) } };

    return std::make_unique<Mesh>(vertexLayout, vertexDataBuffer, indices, rootNode, meshParts);
}

std::unique_ptr<prev_test::render::IMesh> MeshFactory::CreateFromData(const std::vector<glm::vec3>& vertices, const std::vector<uint32_t>& indices) const
{
    const prev_test::render::VertexLayout vertexLayout{ { prev_test::render::VertexLayoutComponent::VEC3 } };

    prev_test::render::VertexDataBuffer vertexDataBuffer;
    for (size_t i = 0; i < vertices.size(); ++i) {
        AddVertexData(vertexDataBuffer, vertices[i]);
    }

    const MeshNode rootNode{ { 0 }, glm::mat4{ 1.0f }, {} };
    const std::vector<prev_test::render::MeshPart> meshParts = { { static_cast<uint32_t>(indices.size()), vertices } };

    return std::make_unique<Mesh>(vertexLayout, vertexDataBuffer, indices, rootNode, meshParts);
}

std::unique_ptr<prev_test::render::IMesh> MeshFactory::CreateFromData(const std::vector<glm::vec2>& vertices, const std::vector<glm::vec2>& textureCoords, const std::vector<uint32_t>& indices) const
{
    const prev_test::render::VertexLayout vertexLayout{ { prev_test::render::VertexLayoutComponent::VEC2, prev_test::render::VertexLayoutComponent::VEC2 } };

    prev_test::render::VertexDataBuffer vertexDataBuffer;
    for (size_t i = 0; i < vertices.size(); ++i) {
        AddVertexData(vertexDataBuffer, vertices[i], textureCoords[i]);
    }

    const MeshNode rootNode{ { 0 }, glm::mat4{ 1.0f }, {} };
    const std::vector<prev_test::render::MeshPart> meshParts = { { static_cast<uint32_t>(indices.size()), ConvertToVec3(vertices) } };

    return std::make_unique<Mesh>(vertexLayout, vertexDataBuffer, indices, rootNode, meshParts);
}

std::unique_ptr<prev_test::render::IMesh> MeshFactory::CreateFromData(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& textureCoords, const std::vector<uint32_t>& indices) const
{
    const prev_test::render::VertexLayout vertexLayout{ { prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC2 } };

    prev_test::render::VertexDataBuffer vertexDataBuffer;
    for (size_t i = 0; i < vertices.size(); ++i) {
        AddVertexData(vertexDataBuffer, vertices[i], textureCoords[i]);
    }

    const MeshNode rootNode{ { 0 }, glm::mat4{ 1.0f }, {} };
    const std::vector<prev_test::render::MeshPart> meshParts = { { static_cast<uint32_t>(indices.size()), vertices } };

    return std::make_unique<Mesh>(vertexLayout, vertexDataBuffer, indices, rootNode, meshParts);
}

std::unique_ptr<prev_test::render::IMesh> MeshFactory::CreateFromData(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<uint32_t>& indices) const
{
    const prev_test::render::VertexLayout vertexLayout{ { prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC3 } };

    prev_test::render::VertexDataBuffer vertexDataBuffer;
    for (size_t i = 0; i < vertices.size(); ++i) {
        AddVertexData(vertexDataBuffer, vertices[i], normals[i]);
    }

    const MeshNode rootNode{ { 0 }, glm::mat4{ 1.0f }, {} };
    const std::vector<prev_test::render::MeshPart> meshParts = { { static_cast<uint32_t>(indices.size()), vertices } };

    return std::make_unique<Mesh>(vertexLayout, vertexDataBuffer, indices, rootNode, meshParts);
}

std::unique_ptr<prev_test::render::IMesh> MeshFactory::CreateFromData(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& textureCoords, const std::vector<glm::vec3>& normals, const std::vector<uint32_t>& indices) const
{
    const prev_test::render::VertexLayout vertexLayout{ { prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC2, prev_test::render::VertexLayoutComponent::VEC3 } };

    prev_test::render::VertexDataBuffer vertexDataBuffer;
    for (size_t i = 0; i < vertices.size(); ++i) {
        AddVertexData(vertexDataBuffer, vertices[i], textureCoords[i], normals[i]);
    }

    const MeshNode rootNode{ { 0 }, glm::mat4{ 1.0f }, {} };
    const std::vector<prev_test::render::MeshPart> meshParts = { { static_cast<uint32_t>(indices.size()), vertices } };

    return std::make_unique<Mesh>(vertexLayout, vertexDataBuffer, indices, rootNode, meshParts);
}

std::unique_ptr<prev_test::render::IMesh> MeshFactory::CreateFromData(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& textureCoords, const std::vector<glm::vec3>& normals, const std::vector<glm::vec3>& tangents, const std::vector<glm::vec3>& biTangents, const std::vector<uint32_t>& indices) const
{
    const prev_test::render::VertexLayout vertexLayout{ { prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC2, prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC3 } };

    prev_test::render::VertexDataBuffer vertexDataBuffer;
    for (size_t i = 0; i < vertices.size(); ++i) {
        AddVertexData(vertexDataBuffer, vertices[i], textureCoords[i], normals[i], tangents[i], biTangents[i]);
    }

    const MeshNode rootNode{ { 0 }, glm::mat4{ 1.0f }, {} };
    const std::vector<prev_test::render::MeshPart> meshParts = { { static_cast<uint32_t>(indices.size()), vertices } };

    return std::make_unique<Mesh>(vertexLayout, vertexDataBuffer, indices, rootNode, meshParts);
}
} // namespace prev_test::render::mesh