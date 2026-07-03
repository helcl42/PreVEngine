#include "MasterRenderer.h"

#include "../Tags.h"
#include "../component/CameraComponent.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace sandbox::render {
namespace {
    // The camera builds projections in the Vulkan convention (Y already flipped). WebGPU clip space
    // is Y-up, so undo that flip there; every other backend passes through unchanged.
    glm::mat4 AdjustProjectionForBackend(glm::mat4 projection, GfxBackend backend)
    {
        if (backend == GFX_BACKEND_WEBGPU) {
            projection[1][1] *= -1.0f;
        }
        return projection;
    }
} // namespace

MasterRenderer::MasterRenderer(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene)
    : m_device{ device }
    , m_renderPass{ renderPass }
    , m_scene{ scene }
{
}

void MasterRenderer::Init()
{
    m_defaultRenderer = std::make_unique<DefaultRenderer>(m_device, m_renderPass);
    m_defaultRenderer->Init();
}

prev::render::FrameSubmitSync MasterRenderer::Render(const prev::render::RenderContext& renderContext, const prev::scene::IScene& scene)
{
    const auto camera{ prev::scene::component::NodeComponentHelper::Find<sandbox::component::CameraComponent>(scene.GetRootNode(), { sandbox::TAG_MAIN_CAMERA }) };

    const GfxBackend backend{ m_device.GetGPU().GetInfo().backend };

    ViewData views{};
    views.viewCount = renderContext.viewCount;
    for (uint32_t slot = 0; slot < views.viewCount; ++slot) {
        const uint32_t eye{ renderContext.viewOffset + slot };
        views.viewMatrices[slot] = camera->GetViewMatrix(eye);
        views.projectionMatrices[slot] = AdjustProjectionForBackend(camera->GetProjectionMatrix(eye), backend);
    }

    // Reset the renderer's bind-group pool to this frame's region (grow-on-demand), once per frame.
    m_defaultRenderer->BeginFrame(renderContext.frameInFlightIndex);

    m_renderPass.Begin(renderContext.frameBuffer, renderContext.commandEncoder);

    const GfxRenderPassEncoder encoder{ m_renderPass.GetEncoder() };
    TraverseScene(encoder, renderContext.rect, views, scene.GetRootNode());

    m_renderPass.End();

    m_defaultRenderer->EndFrame(); // trim this frame's bind-group region back to what it used

    return {};
}

void MasterRenderer::TraverseScene(GfxRenderPassEncoder encoder, const GfxScissorRect& rect, const ViewData& views, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    m_defaultRenderer->Render(encoder, rect, views, node);

    for (const auto& child : node->GetChildren()) {
        TraverseScene(encoder, rect, views, child);
    }
}

void MasterRenderer::ShutDown()
{
    if (m_defaultRenderer) {
        m_defaultRenderer->ShutDown();
        m_defaultRenderer.reset();
    }
}
} // namespace sandbox::render
