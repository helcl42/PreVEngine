#ifndef __SUN_H__
#define __SUN_H__

#include "../../General.h"
#include "../../component/sky/ISunComponent.h"

#include <prev/core/CoreEvents.h>
#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/event/EventHandler.h>
#include <prev/scene/graph/SceneNode.h>

#include <vector>

namespace prev_test::scene::sky {
class Sun final : public prev::scene::graph::SceneNode {
public:
    Sun(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, uint32_t viewCount);

    ~Sun() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

public:
    void operator()(const prev::core::NewIterationEvent& newIterationEvent);

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

    uint32_t m_viewCount;

private:
    std::vector<std::shared_ptr<prev_test::component::sky::ISunComponent>> m_sunComponents;

    glm::vec2 m_viewPortSize{};

private:
    prev::event::EventHandler<Sun, prev::core::NewIterationEvent> m_newIterationHandler{ *this };
};
} // namespace prev_test::scene::sky

#endif // !__SUN_H__
