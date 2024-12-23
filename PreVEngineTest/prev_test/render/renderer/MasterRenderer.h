#ifndef __MASTER_RENDERER_H__
#define __MASTER_RENDERER_H__

#include "CommandBuffersGroup.h"
#include "IRenderer.h"
#include "RenderContexts.h"

#include "../../General.h"

#include <prev/common/ThreadPool.h>
#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/event/EventHandler.h>
#include <prev/input/keyboard/KeyboardEvents.h>
#include <prev/render/IRootRenderer.h>
#include <prev/render/Swapchain.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/scene/graph/ISceneNode.h>

namespace prev_test::render::renderer {
class MasterRenderer final : public prev::render::IRootRenderer {
public:
    MasterRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass, uint32_t swapchainImageCount, uint32_t viewCount);

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
    void RenderParallel(prev::render::pass::RenderPass& renderPass, const RenderContextType& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& root, const std::vector<std::unique_ptr<IRenderer<RenderContextType>>>& renderers, const std::vector<VkCommandBuffer>& commandBuffers);
#else
    template <typename RenderContextType>
    void RenderSerial(prev::render::pass::RenderPass& renderPass, const RenderContextType& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& root, const std::vector<std::unique_ptr<IRenderer<RenderContextType>>>& renderers);
#endif
private:
    static const inline glm::vec4 DEFAULT_CLIP_PLANE{ 0.0f, -1.0f, 0.0f, 100000.0f };

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

    prev::render::pass::RenderPass& m_defaultRenderPass;

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
void MasterRenderer::RenderParallel(prev::render::pass::RenderPass& renderPass, const RenderContextType& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& root, const std::vector<std::unique_ptr<IRenderer<RenderContextType>>>& renderers, const std::vector<VkCommandBuffer>& commandBuffers)
{
    for (auto& renderer : renderers) {
        renderer->BeforeRender(renderContext);
    }

    renderPass.Begin(renderContext.frameBuffer, renderContext.commandBuffer, renderContext.rect, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

    std::vector<std::future<void>> tasks;
    tasks.reserve(renderers.size());

    for (size_t i = 0; i < renderers.size(); ++i) {
        auto& renderer{ renderers[i] };
        auto& commandBuffer{ commandBuffers[i] };

        tasks.emplace_back(m_threadPool.Enqueue([&]() {
            VkCommandBufferInheritanceInfo inheritanceInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO };
            inheritanceInfo.renderPass = renderPass;
            inheritanceInfo.framebuffer = renderContext.frameBuffer;

            VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
            commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
            commandBufferBeginInfo.pInheritanceInfo = &inheritanceInfo;

            VKERRCHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

            RenderContextType parallelRenderContext{ renderContext };
            parallelRenderContext.commandBuffer = commandBuffer;

            renderer->PreRender(parallelRenderContext);

            TraverseScene(parallelRenderContext, root, renderer);

            renderer->PostRender(parallelRenderContext);

            VKERRCHECK(vkEndCommandBuffer(commandBuffer));
        }));
    }

    for (auto&& task : tasks) {
        task.get();
    }

    vkCmdExecuteCommands(renderContext.commandBuffer, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

    renderPass.End(renderContext.commandBuffer);

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

    renderPass.Begin(renderContext.frameBuffer, renderContext.commandBuffer, renderContext.rect);

    for (auto& renderer : renderers) {
        renderer->PreRender(renderContext);

        TraverseScene(renderContext, root, renderer);

        renderer->PostRender(renderContext);
    }

    renderPass.End(renderContext.commandBuffer);

    for (auto& renderer : renderers) {
        renderer->AfterRender(renderContext);
    }
}
#endif

} // namespace prev_test::render::renderer

#endif // !__MASTER_RENDERER_H__
