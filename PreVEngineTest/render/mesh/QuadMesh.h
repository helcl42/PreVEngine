#ifndef __QUAD_MESH_H__
#define __QUAD_MESH_H__

#include "../IMesh.h"
#include "../VertexDataBuffer.h"

namespace prev_test::render::mesh {
class QuadMesh final : public prev_test::render::IMesh {
public:
    QuadMesh(const bool includeTangentBiTangent = false);

    ~QuadMesh() = default;

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
        { 1.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f }
    };

    static const inline std::vector<glm::vec2> textureCoords{
        { 1.0f, 1.0f },
        { 0.0f, 1.0f },
        { 0.0f, 0.0f },
        { 1.0f, 0.0f }
    };

    static const inline std::vector<glm::vec3> normals{
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f }
    };

    static const inline std::vector<uint32_t> indices{
        0, 1, 2, 2, 3, 0
    };

    static const inline std::vector<prev_test::render::MeshPart> meshParts = {
        prev_test::render::MeshPart(static_cast<uint32_t>(indices.size()))
    };

    static const inline prev_test::render::MeshNode meshRootNode{ { 0 }, glm::mat4(1.0f), {} };
};
} // namespace prev_test::render::mesh

#endif // !__QUAD_MESH_H__
