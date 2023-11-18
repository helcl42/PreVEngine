#ifndef __TERRAIN_COMPONENT_FACTORY_H__
#define __TERRAIN_COMPONENT_FACTORY_H__

#include "HeightGenerator.h"
#include "ITerrainComponent.h"
#include "TerrainLayerCreateInfo.h"

#include <map>

namespace prev_test::component::terrain {
class TerrainComponentFactory {
public:
    TerrainComponentFactory(const unsigned int seed = 21236728, const unsigned int vertexCount = 28);

    ~TerrainComponentFactory() = default;

public:
    std::unique_ptr<ITerrainComponenet> CreateRandomTerrain(const int x, const int z, const float size) const;

    std::unique_ptr<ITerrainComponenet> CreateRandomTerrainNormalMapped(const int x, const int z, const float size) const;

    std::unique_ptr<ITerrainComponenet> CreateRandomTerrainParallaxMapped(const int x, const int z, const float size) const;

    std::unique_ptr<ITerrainComponenet> CreateRandomTerrainConeStepMapped(const int x, const int z, const float size) const;

private:
    std::unique_ptr<prev_test::render::IModel> CreateModel(prev::core::memory::Allocator& allocator, const std::shared_ptr<VertexData>& vertexData, const bool normalMapped) const;

    std::unique_ptr<prev_test::render::IMesh> GenerateMesh(const std::shared_ptr<VertexData>& vertexData, const bool normalMapped) const;

    std::unique_ptr<VertexData> GenerateVertexData(const std::shared_ptr<HeightMapInfo>& heightMap, const float size) const;

    glm::vec3 CalculatePosition(const std::shared_ptr<HeightMapInfo>& heightMap, const int x, const int z, const float size) const;

    glm::vec2 CalculateTextureCoordinates(const int x, const int z) const;

    std::unique_ptr<HeightMapInfo> CreateHeightMap(const HeightGenerator& generator) const;

private:
    const unsigned int m_seed;

    const unsigned int m_vertexCount;
};
} // namespace prev_test::component::terrain

#endif // !__TERRAIN_COMPONENT_FACTORY_H__
