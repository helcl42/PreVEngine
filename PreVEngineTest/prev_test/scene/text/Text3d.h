#ifndef __TEXT_3D_H__
#define __TEXT_3D_H__

#include "../../component/font/IFontRenderComponent.h"
#include "../../render/font/WorldSpaceText.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/scene/graph/SceneNode.h>
#include <prev/util/Utils.h>

namespace prev_test::scene::text {
class Text3d final : public prev::scene::graph::SceneNode {
public:
    Text3d(prev::core::device::Device& device, prev::core::memory::Allocator& allocator);

    ~Text3d() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

private:
    std::shared_ptr<prev_test::component::font::IFontRenderComponent<prev_test::render::font::WorldSpaceText>> m_fontComponent;

    prev::util::FPSCounter m_fpsCounter{ 0.1f };
};
} // namespace prev_test::scene::text

#endif // !__TEXT_H__
