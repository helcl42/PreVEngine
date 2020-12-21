#ifndef __CUBE_MESH_H__
#define __CUBE_MESH_H__

#include "../IMesh.h"
#include "../VertexDataBuffer.h"

namespace prev_test::render::mesh {
class CubeMesh final : public prev_test::render::IMesh {
public:
    CubeMesh(const bool includeTangentBiTangent = false);

    ~CubeMesh() = default;

public:
    const prev_test::render::VertexLayout& GetVertexLayout() const override;

    const void* GetVertexData() const override;

    const std::vector<glm::vec3>& GetVertices() const override;

    uint32_t GerVerticesCount() const override;

    const std::vector<uint32_t>& GetIndices() const override;

    const std::vector<prev_test::render::MeshPart>& GetMeshParts() const override;

    const MeshNode& GetRootNode() const override;

private:
    prev_test::render::VertexLayout m_vertexLayout;

    prev_test::render::VertexDataBuffer m_vertexDataBuffer;

private:
    static const inline std::vector<glm::vec3> vertices{
        // FROMT
        { -0.5f, -0.5f, 0.5f },
        { 0.5f, -0.5f, 0.5f },
        { 0.5f, 0.5f, 0.5f },
        { -0.5f, 0.5f, 0.5f },
        // BACK
        { -0.5f, -0.5f, -0.5f },
        { 0.5f, -0.5f, -0.5f },
        { 0.5f, 0.5f, -0.5f },
        { -0.5f, 0.5f, -0.5f },
        // TOP
        { -0.5f, 0.5f, 0.5f },
        { 0.5f, 0.5f, 0.5f },
        { 0.5f, 0.5f, -0.5f },
        { -0.5f, 0.5f, -0.5f },
        // BOTTOM
        { -0.5f, -0.5f, 0.5f },
        { 0.5f, -0.5f, 0.5f },
        { 0.5f, -0.5f, -0.5f },
        { -0.5f, -0.5f, -0.5f },
        // LEFT
        { -0.5f, -0.5f, 0.5f },
        { -0.5f, 0.5f, 0.5f },
        { -0.5f, 0.5f, -0.5f },
        { -0.5f, -0.5f, -0.5f },
        // RIGHT
        { 0.5f, -0.5f, 0.5f },
        { 0.5f, 0.5f, 0.5f },
        { 0.5f, 0.5f, -0.5f },
        { 0.5f, -0.5f, -0.5f }
    };

    static const inline std::vector<glm::vec2> textureCoords{
        // FROMT
        { 1.0f, 0.0f },
        { 0.0f, 0.0f },
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        // BACK
        { 1.0f, 0.0f },
        { 0.0f, 0.0f },
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        // TOP
        { 1.0f, 0.0f },
        { 0.0f, 0.0f },
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        // BOTTOM
        { 1.0f, 0.0f },
        { 0.0f, 0.0f },
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        // LEFT
        { 1.0f, 0.0f },
        { 0.0f, 0.0f },
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        // RIGHT
        { 1.0f, 0.0f },
        { 0.0f, 0.0f },
        { 0.0f, 1.0f },
        { 1.0f, 1.0f }
    };

    static const inline std::vector<glm::vec3> normals{
        // FROMT
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
        // BACK
        { 0.0f, 0.0f, -1.0f },
        { 0.0f, 0.0f, -1.0f },
        { 0.0f, 0.0f, -1.0f },
        { 0.0f, 0.0f, -1.0f },
        // TOP
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        // BOTTOM
        { 0.0f, -1.0f, 0.0f },
        { 0.0f, -1.0f, 0.0f },
        { 0.0f, -1.0f, 0.0f },
        { 0.0f, -1.0f, 0.0f },
        // LEFT
        { -1.0f, 0.0f, 0.0f },
        { -1.0f, 0.0f, 0.0f },
        { -1.0f, 0.0f, 0.0f },
        { -1.0f, 0.0f, 0.0f },
        // RIGHT
        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
    };

    static const inline std::vector<uint32_t> indices{
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };

    static const inline std::vector<prev_test::render::MeshPart> meshParts = {
        prev_test::render::MeshPart(static_cast<uint32_t>(indices.size()))
    };

    static const inline prev_test::render::MeshNode meshRootNode{ { 0 }, glm::mat4(1.0f), {} };
};
} // namespace prev_test::render::mesh

#endif // !__CUBE_MESH_H__
