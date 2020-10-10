#ifndef __WATER_TILE_MESH_H__
#define __WATER_TILE_MESH_H__

#include "../../render/IMesh.h"

namespace prev_test::component::water {
class WaterTileMesh final : public prev_test::render::IMesh {
public:
    const prev_test::render::VertexLayout& GetVertexLayout() const override;

    const void* GetVertexData() const override;

    const std::vector<glm::vec3>& GetVertices() const override;

    uint32_t GerVerticesCount() const override;

    const std::vector<uint32_t>& GetIndices() const override;

    const std::vector<prev_test::render::MeshPart>& GetMeshParts() const override;

private:
    static const inline prev_test::render::VertexLayout vertexLayout{ { prev_test::render::VertexLayoutComponent::VEC3 } };

    static const inline std::vector<glm::vec3> vertices = {
        { 1.0f, 0.0f, 1.0f },
        { -1.0f, 0.0f, 1.0f },
        { -1.0f, 0.0f, -1.0f },
        { 1.0f, 0.0f, -1.0f }
    };

    static const inline std::vector<uint32_t> indices = {
        0, 1, 2, 2, 3, 0
    };

    static const inline std::vector<prev_test::render::MeshPart> meshParts = {
        prev_test::render::MeshPart(static_cast<uint32_t>(indices.size()))
    };
};
} // namespace prev_test::component::water

#endif // !__WATER_TILE_MESH_H__
