#ifndef __SHADOW_H__
#define __SHADOW_H__

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene::shadow {
class Shadows final : public prev::scene::graph::SceneNode {
public:
    Shadows(prev::core::device::Device& device, prev::core::memory::Allocator& allocator);

    ~Shadows() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;
};
} // namespace prev_test::scene::shadow

#endif // !__SHADOW_H__
