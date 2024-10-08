#ifndef __TEXT_H__
#define __TEXT_H__

#include "../../General.h"
#include "../../component/font/IFontRenderComponent.h"
#include "../../render/font/ScreenSpaceText.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/scene/graph/SceneNode.h>
#include <prev/util/Utils.h>

namespace prev_test::scene::text {
class Text final : public prev::scene::graph::SceneNode {
public:
    Text(prev::core::device::Device& device, prev::core::memory::Allocator& allocator);

    ~Text() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

private:
    std::shared_ptr<prev_test::component::font::IFontRenderComponent<prev_test::render::font::ScreenSpaceText>> m_fontComponent;

    prev::util::FPSCounter m_fpsCounter{ 0.1f };
};
} // namespace prev_test::scene::text

#endif // !__TEXT_H__
