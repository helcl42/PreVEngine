#ifndef __SANDBOX_MASTER_RENDERER_H__
#define __SANDBOX_MASTER_RENDERER_H__

#include "DefaultRenderer.h"

#include "../common/Common.h"

#include <prev/core/device/Device.h>
#include <prev/render/IRootRenderer.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/scene/IScene.h>
#include <prev/scene/graph/ISceneNode.h>

#include <glm/glm.hpp>

#include <array>
#include <cstdint>
#include <memory>

namespace sandbox::render {
// Root renderer: drives the single render pass, finds the camera, and draws the scene's sandboxs.
class MasterRenderer final : public prev::render::IRootRenderer {
public:
    MasterRenderer(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene);

    ~MasterRenderer() = default;

public:
    void Init() override;

    prev::render::FrameSubmitSync Render(const prev::render::RenderContext& renderContext, const prev::scene::IScene& scene) override;

    void ShutDown() override;

private:
    void TraverseScene(GfxRenderPassEncoder encoder, const GfxScissorRect& rect, const ViewData& views, const std::shared_ptr<prev::scene::graph::ISceneNode>& node);

private:
    prev::core::device::Device& m_device;
    prev::render::pass::RenderPass& m_renderPass;
    prev::scene::IScene& m_scene;

    std::unique_ptr<DefaultRenderer> m_defaultRenderer;
};
} // namespace sandbox::render

#endif // !__SANDBOX_MASTER_RENDERER_H__
