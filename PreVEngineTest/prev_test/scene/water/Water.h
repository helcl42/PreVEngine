#ifndef __WATER_H__
#define __WATER_H__

#include "../../General.h"
#include "../../component/ray_casting/IBoundingVolumeComponent.h"
#include "../../component/transform/ITransformComponent.h"
#include "../../component/water/IWaterComponent.h"

#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene::water {
class Water final : public prev::scene::graph::SceneNode {
public:
    Water(const int x, const int z);

    ~Water() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    const int m_x;

    const int m_z;

    std::shared_ptr<prev_test::component::transform::ITransformComponent> m_transformComponent;

    std::shared_ptr<prev_test::component::water::IWaterComponent> m_waterComponent;

    std::shared_ptr<prev_test::component::ray_casting::IBoundingVolumeComponent> m_boundingVolumeComponent;
};
} // namespace prev_test::scene::water

#endif // !__WATER_H__
