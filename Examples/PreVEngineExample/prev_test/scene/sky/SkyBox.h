#ifndef __SKY_BOX_H__
#define __SKY_BOX_H__

#include "../../component/sky/ISkyBoxComponent.h"
#include "../../component/transform/ITransformComponent.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene::sky {
class SkyBox final : public prev::scene::graph::SceneNode {
public:
    SkyBox(prev::core::device::Device& device, prev::core::memory::Allocator& allocator);

    ~SkyBox() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

private:
    std::shared_ptr<prev_test::component::transform::ITransformComponent> m_transformComponent;

    std::shared_ptr<prev_test::component::sky::ISkyBoxComponent> m_skyBoxComponent;

    static const inline float ROTATION_SPEED_DEGS_PER_SEC{ 0.5f };
};
} // namespace prev_test::scene::sky

#endif // !__SKY_BOX_H__
