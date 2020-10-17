#ifndef __TERRAIN_MANAGER_H__
#define __TERRAIN_MANAGER_H__

#include "../../General.h"
#include "../../component/terrain/TerrainCommon.h"

#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene::terrain {
class TerrainManager final : public prev::scene::graph::SceneNode<SceneNodeFlags> {
public:
    TerrainManager(const uint32_t maxX, const uint32_t maxZ);

    virtual ~TerrainManager() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    const uint32_t m_gridMaxX;

    const uint32_t m_gridMaxZ;
};
} // namespace prev_test::scene::terrain

#endif // !__TERRAIN_MANAGER_H__
