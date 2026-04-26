#ifndef __MASTER_RENDERER_H__
#define __MASTER_RENDERER_H__

#include "CommandBuffersGroup.h"
#include "IRenderer.h"
#include "RenderContexts.h"

#include <prev/common/ThreadPool.h>
#include <prev/core/device/Device.h>
#include <prev/event/EventHandler.h>
#include <prev/input/keyboard/KeyboardEvents.h>
#include <prev/render/IRootRenderer.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/scene/IScene.h>
#include <prev/scene/graph/ISceneNode.h>

namespace prev_test::render::renderer {
class MasterRenderer final : public prev::render::IRootRenderer {
public:
    MasterRenderer(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene, uint32_t swapchainImageCount, uint32_t viewCount);

    ~MasterRenderer() = default;

public:
    void Init() override;

    void Render(const prev::render::RenderContext& renderContext, const prev::scene::IScene& scene) override;

    void ShutDown() override;

public:
    void operator()(const prev::input::keyboard::KeyEvent& keyEvent);

private:
    void InitDefault();

    void ShutDownDefault();

    void InitDebug();

    void ShutDownDebug();

    void InitShadows();

    void ShutDownShadows();

    void InitReflection();

    void ShutDownReflection();

    void InitRefraction();

    void ShutDownRefraction();

    void RenderShadows(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& root);

    void RenderSceneReflection(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& root);

    void RenderSceneRefraction(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& root);

    void RenderScene(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& root);

    void RenderDebug(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& root);

    template <typename RenderContextType>
    void TraverseScene(const RenderContextType& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const std::unique_ptr<IRenderer<RenderContextType>>& renderer);

#ifdef PARALLEL_RENDERING
    template <typename RenderContextType>
    void RenderParallel(prev::render::pass::RenderPass& renderPass, const RenderContextType& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& root, const std::vector<std::unique_ptr<IRenderer<RenderContextType>>>& renderers, const std::vector<void*>& commandBuffers);
#else
    template <typename RenderContextType>
    void RenderSerial(prev::render::pass::RenderPass& renderPass, const RenderContextType& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& root, const std::vector<std::unique_ptr<IRenderer<RenderContextType>>>& renderers);
#endif
private:
    static const inline glm::vec4 DEFAULT_CLIP_PLANE{ 0.0f, -1.0f, 0.0f, 100000.0f };

private:
    prev::core::device::Device& m_device;


    prev::render::pass::RenderPass& m_defaultRenderPass;

    prev::scene::IScene& m_scene;

    uint32_t m_swapchainImageCount;

    uint32_t m_viewCount;

private:
    // Default
    std::vector<std::unique_ptr<IRenderer<NormalRenderContext>>> m_defaultRenderers;

    // Debug
    std::vector<std::unique_ptr<IRenderer<prev::render::RenderContext>>> m_debugRenderers;

    // Shadows
    std::vector<std::unique_ptr<IRenderer<ShadowsRenderContext>>> m_shadowRenderers;

    // Reflection
    std::vector<std::unique_ptr<IRenderer<NormalRenderContext>>> m_reflectionRenderers;

    // Refraction
    std::vector<std::unique_ptr<IRenderer<NormalRenderContext>>> m_refractionRenderers;

#ifdef PARALLEL_RENDERING
    // Parallel stuff
    std::unique_ptr<CommandBuffersGroup> m_defaultCommandBuffersGroup;

    std::unique_ptr<CommandBuffersGroup> m_debugCommandBuffersGroup;

    std::vector<std::unique_ptr<CommandBuffersGroup>> m_shadowsCommandBufferGroups;

    std::unique_ptr<CommandBuffersGroup> m_reflectionCommandBufferGroups;

    std::unique_ptr<CommandBuffersGroup> m_refractionCommandBufferGroups;

    prev::common::ThreadPool m_threadPool{ std::thread::hardware_concurrency() };
#endif

    prev::event::EventHandler<MasterRenderer, prev::input::keyboard::KeyEvent> m_keyboardEventHandler{ *this };
};

template <typename RenderContextType>
void MasterRenderer::TraverseScene(const RenderContextType& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const std::unique_ptr<IRenderer<RenderContextType>>& renderer)
{
    renderer->Render(renderContext, node);

    for (const auto& child : node->GetChildren()) {
        TraverseScene(renderContext, child, renderer);
    }
}

#ifdef PARALLEL_RENDERING
template <typename RenderContextType>
void MasterRenderer::RenderParallel(prev::render::pass::RenderPass& renderPass, const RenderContextType& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& root, const std::vector<std::unique_ptr<IRenderer<RenderContextType>>>& renderers, const std::vector<void*>& commandBuffers)
{
    // TODO: Parallel rendering with secondary command buffers is not yet supported in gfx API
    // Fallback to serial path
    for (auto& renderer : renderers) {
        renderer->BeforeRender(renderContext);
    }

    renderPass.Begin(renderContext.frameBuffer, renderContext.commandEncoder);

    RenderContextType passContext{ renderContext };
    passContext.renderPassEncoder = renderPass.GetEncoder();

    for (auto& renderer : renderers) {
        renderer->PreRender(passContext);

        TraverseScene(passContext, root, renderer);

        renderer->PostRender(passContext);
    }

    renderPass.End();

    for (auto& renderer : renderers) {
        renderer->AfterRender(renderContext);
    }
}
#else
template <typename RenderContextType>
void MasterRenderer::RenderSerial(prev::render::pass::RenderPass& renderPass, const RenderContextType& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& root, const std::vector<std::unique_ptr<IRenderer<RenderContextType>>>& renderers)
{
    for (auto& renderer : renderers) {
        renderer->BeforeRender(renderContext);
    }

    renderPass.Begin(renderContext.frameBuffer, renderContext.commandEncoder);

    RenderContextType passContext{ renderContext };
    passContext.renderPassEncoder = renderPass.GetEncoder();

    for (auto& renderer : renderers) {
        renderer->PreRender(passContext);

        TraverseScene(passContext, root, renderer);

        renderer->PostRender(passContext);
    }

    renderPass.End();

    for (auto& renderer : renderers) {
        renderer->AfterRender(renderContext);
    }
}
#endif

} // namespace prev_test::render::renderer

#endif // !__MASTER_RENDERER_H__
