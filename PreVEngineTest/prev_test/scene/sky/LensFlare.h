#ifndef __LENS_FLARE_H__
#define __LENS_FLARE_H__

#include "../../General.h"
#include "../../component/sky/ILensFlareComponent.h"

#include <prev/core/CoreEvents.h>
#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/event/EventHandler.h>
#include <prev/scene/graph/SceneNode.h>

#include <vector>

namespace prev_test::scene::sky {
class LensFlare final : public prev::scene::graph::SceneNode {
public:
    LensFlare(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, uint32_t viewCount);

    ~LensFlare() = default;

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
    std::vector<std::shared_ptr<prev_test::component::sky::ILensFlareComponent>> m_lensFlareComponents;

    glm::vec2 m_viewPortSize{};

private:
    prev::event::EventHandler<LensFlare, prev::core::NewIterationEvent> m_newIterationHandler{ *this };
};
} // namespace prev_test::scene::sky

#endif // !__LENS_FLARE_H__