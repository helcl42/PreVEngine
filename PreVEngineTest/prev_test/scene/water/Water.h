#ifndef __WATER_H__
#define __WATER_H__

#include "../../component/ray_casting/IBoundingVolumeComponent.h"
#include "../../component/transform/ITransformComponent.h"
#include "../../component/water/IWaterComponent.h"

#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene::water {
class Water final : public prev::scene::graph::SceneNode {
public:
    Water(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, const int x, const int z);

    ~Water() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

    int m_x;

    int m_z;

    std::shared_ptr<prev_test::component::transform::ITransformComponent> m_transformComponent;

    std::shared_ptr<prev_test::component::water::IWaterComponent> m_waterComponent;

    std::shared_ptr<prev_test::component::ray_casting::IBoundingVolumeComponent> m_boundingVolumeComponent;
};
} // namespace prev_test::scene::water

#endif // !__WATER_H__
