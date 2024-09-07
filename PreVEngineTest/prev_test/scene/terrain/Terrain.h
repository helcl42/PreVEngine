#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include "../../General.h"
#include "../../component/ray_casting/IBoundingVolumeComponent.h"
#include "../../component/terrain/ITerrainComponent.h"
#include "../../component/terrain/ITerrainManagerComponent.h"
#include "../../component/terrain/TerrainCommon.h"
#include "../../component/transform/ITransformComponent.h"

#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene::terrain {
class Terrain final : public prev::scene::graph::SceneNode {
public:
    Terrain(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, const int x, const int z);

    ~Terrain() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

    int m_xIndex;

    int m_zIndex;

private:
    std::shared_ptr<prev_test::component::transform::ITransformComponent> m_transformComponent;

    std::shared_ptr<prev_test::component::terrain::ITerrainComponenet> m_terrainComponent;

    std::weak_ptr<prev_test::component::terrain::ITerrainManagerComponent> m_terrainManagerComponent;

    std::shared_ptr<prev_test::component::ray_casting::IBoundingVolumeComponent> m_boundingVolumeComponent;
};
} // namespace prev_test::scene::terrain

#endif // !__TERRAIN_H__
