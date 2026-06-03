#ifndef __HAND_TRACKING_H__
#define __HAND_TRACKING_H__

#ifdef ENABLE_XR

#include <prev/core/device/Device.h>
#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene {
class HandTracking final : public prev::scene::graph::SceneNode {
public:
    HandTracking(prev::core::device::Device& device);

    ~HandTracking() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    prev::core::device::Device& m_device;
};
} // namespace prev_test::scene

#endif

#endif // !__HAND_TRACKING_H__
