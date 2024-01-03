#ifndef __TEXT_MESH_H__
#define __TEXT_MESH_H__

#include "../IMesh.h"
#include "../VertexDataBuffer.h"

namespace prev_test::render::font {
class TextMesh final : public prev_test::render::IMesh {
public:
    TextMesh(const std::vector<glm::vec2>& vertices, const std::vector<glm::vec2>& textureCoords, const std::vector<uint32_t>& indices);

    ~TextMesh() = default;

public:
    const prev_test::render::VertexLayout& GetVertexLayout() const override;

    const void* GetVertexData() const override;

    uint32_t GerVerticesCount() const override;

    const std::vector<uint32_t>& GetIndices() const override;

    const std::vector<prev_test::render::MeshPart>& GetMeshParts() const override;

    const MeshNode& GetRootNode() const override;

private:
    prev_test::render::VertexLayout m_vertexLayout;

    std::vector<glm::vec3> m_vertices;

    uint32_t m_verticesCount;

    std::vector<uint32_t> m_indices;

    prev_test::render::VertexDataBuffer m_vertexDataBuffer;

    std::vector<prev_test::render::MeshPart> m_meshParts;

    static const inline prev_test::render::MeshNode meshRootNode{ { 0 }, glm::mat4(1.0f), {} };
};
} // namespace prev_test::render::font

#endif // !__TEXT_MESH_H__
