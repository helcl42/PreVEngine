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
class Terrain final : public prev::scene::graph::SceneNode<SceneNodeFlags> {
public:
    Terrain(const int x, const int z);

    virtual ~Terrain() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    const int m_xIndex;

    const int m_zIndex;

    std::shared_ptr<prev_test::component::transform::ITransformComponent> m_transformComponent;

    std::shared_ptr<prev_test::component::terrain::ITerrainComponenet> m_terrainComponent;

    std::weak_ptr<prev_test::component::terrain::ITerrainManagerComponent> m_terrainManagerComponent;

    std::shared_ptr<prev_test::component::ray_casting::IBoundingVolumeComponent> m_boundingVolumeComponent;
};
} // namespace prev_test::scene::terrain

#endif // !__TERRAIN_H__
