#ifndef __SKY_H__
#define __SKY_H__

#include "../../General.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene::sky {
class Sky final : public prev::scene::graph::SceneNode {
public:
    Sky(prev::core::device::Device& device, prev::core::memory::Allocator& allocator);

    ~Sky() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;
};
} // namespace prev_test::scene::sky

#endif // !__SKY_H__
