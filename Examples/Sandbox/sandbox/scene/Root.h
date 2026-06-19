#ifndef __SANDBOX_ROOT_H__
#define __SANDBOX_ROOT_H__

#include <prev/core/device/Device.h>
#include <prev/scene/graph/SceneNode.h>

#include <cstdint>

namespace sandbox::scene {
class Root final : public prev::scene::graph::SceneNode {
public:
    Root(prev::core::device::Device& device, uint32_t viewCount);

    ~Root() = default;

public:
    void Init() override;

private:
    prev::core::device::Device& m_device;
    uint32_t m_viewCount{ 1 };
};
} // namespace sandbox::scene

#endif // !__SANDBOX_ROOT_H__
