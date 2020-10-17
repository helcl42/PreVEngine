#ifndef __WATER_MANAGER_H__
#define __WATER_MANAGER_H__

#include "../../General.h"

#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene::water {
class WaterManager final : public prev::scene::graph::SceneNode<SceneNodeFlags> {
public:
    WaterManager(const int maxX, const int maxZ);

    virtual ~WaterManager() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    const int m_gridMaxX;

    const int m_gridMaxZ;
};
} // namespace prev_test::scene::water

#endif // !__WATER_MANAGER_H__