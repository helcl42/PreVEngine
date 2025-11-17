#ifndef __TERRAIN_COMPONENT_FACTORY_H__
#define __TERRAIN_COMPONENT_FACTORY_H__

#include "HeightGenerator.h"
#include "ITerrainComponent.h"
#include "TerrainLayerCreateInfo.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>

#include <map>

namespace prev_test::component::terrain {
class TerrainComponentFactory {
public:
    TerrainComponentFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, const unsigned int seed = 21236728, const unsigned int vertexCount = 28);

    ~TerrainComponentFactory() = default;

public:
    std::unique_ptr<ITerrainComponent> CreateRandomTerrain(const int x, const int z, const float size) const;

    std::unique_ptr<ITerrainComponent> CreateRandomTerrainNormalMapped(const int x, const int z, const float size) const;

    std::unique_ptr<ITerrainComponent> CreateRandomTerrainConeStepMapped(const int x, const int z, const float size) const;

private:
    struct VertexData {
        std::vector<glm::vec3> vertices;

        std::vector<glm::vec2> textureCoords;

        std::vector<glm::vec3> normals;

        std::vector<glm::vec3> tangents;

        std::vector<glm::vec3> biTangents;

        std::vector<uint32_t> indices;
    };

private:
    std::unique_ptr<prev_test::render::IModel> CreateModel(const std::shared_ptr<VertexData>& vertexData, const bool normalMapped) const;

    std::unique_ptr<prev_test::render::IMesh> GenerateMesh(const std::shared_ptr<VertexData>& vertexData, const bool normalMapped) const;

    std::unique_ptr<VertexData> GenerateVertexData(const std::shared_ptr<HeightMapInfo>& heightMap, const float size) const;

    glm::vec3 CalculatePosition(const std::shared_ptr<HeightMapInfo>& heightMap, const int x, const int z, const float size) const;

    glm::vec2 CalculateTextureCoordinates(const int x, const int z) const;

    std::unique_ptr<HeightMapInfo> CreateHeightMap(const HeightGenerator& generator) const;

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

    unsigned int m_seed;

    unsigned int m_vertexCount;
};
} // namespace prev_test::component::terrain

#endif // !__TERRAIN_COMPONENT_FACTORY_H__
