#ifndef __SKY_H__
#define __SKY_H__

#include "../../component/sky/ISkyComponent.h"

#include <prev/core/device/Device.h>
#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene::sky {
class Sky final : public prev::scene::graph::SceneNode {
public:
    Sky(prev::core::device::Device& device);

    ~Sky() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    prev::core::device::Device& m_device;

    std::shared_ptr<prev_test::component::sky::ISkyComponent> m_skyComponent;
};
} // namespace prev_test::scene::sky

#endif // !__SKY_H__
