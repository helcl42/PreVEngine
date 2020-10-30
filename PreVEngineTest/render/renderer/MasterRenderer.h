#ifndef __MASTER_RENDERER_H__
#define __MASTER_RENDERER_H__

#include "../../General.h"
#include "../pipeline/IPipeline.h"
#include "RenderContextUserData.h"
#include "CommandBuffersGroup.h"

#include "../../General.h"

#include <prev/common/ThreadPool.h>
#include <prev/render/IRenderer.h>
#include <prev/render/Swapchain.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/graph/ISceneNode.h>

//#define PARALLEL_RENDERING

namespace prev_test::render::renderer {
class MasterRenderer final : public prev::render::IRenderer<prev::render::DefaultRenderContextUserData> {
public:
    MasterRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass, const std::shared_ptr<prev::render::Swapchain>& swapchain);

    ~MasterRenderer() = default;

public:
    void Init() override;

    void BeforeRender(const prev::render::RenderContext& renderContext, const prev::render::DefaultRenderContextUserData& renderContextUserData) override;

    void PreRender(const prev::render::RenderContext& renderContext, const prev::render::DefaultRenderContextUserData& renderContextUserData) override;

    void Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode >& node, const prev::render::DefaultRenderContextUserData& renderContextUserData) override;

    void PostRender(const prev::render::RenderContext& renderContext, const prev::render::DefaultRenderContextUserData& renderContextUserData) override;

    void AfterRender(const prev::render::RenderContext& renderContext, const prev::render::DefaultRenderContextUserData& renderContextUserData) override;

    void ShutDown() override;

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

    void RenderShadows(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode >& root);

    void RenderSceneReflection(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode >& root);

    void RenderSceneRefraction(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode >& root);

    void RenderScene(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode >& root);

    void RenderDebug(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode >& root);

#ifdef PARALLEL_RENDERING
    template <typename ContextUserDataType>
    void RenderParallel(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass, const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode >& root, const std::vector<std::unique_ptr<IRenderer<ContextUserDataType> > >& renderers, const std::vector<VkCommandBuffer>& commandBuffers, const ContextUserDataType& userData, const VkRect2D& area);
#else
    template <typename ContextUserDataType>
    void RenderSerial(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass, const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode >& root, const std::vector<std::unique_ptr<IRenderer<ContextUserDataType> > >& renderers, const ContextUserDataType& userData, const VkRect2D& area);
#endif
private:
    static const inline glm::vec4 DEFAULT_CLIP_PLANE{ 0.0f, -1.0f, 0.0f, 1000.0f };

    std::shared_ptr<prev::render::pass::RenderPass> m_defaultRenderPass;

    std::shared_ptr<prev::render::Swapchain> m_swapchain;

private:
    // Default
    std::vector<std::unique_ptr<IRenderer<NormalRenderContextUserData> > > m_defaultRenderers;

    // Debug
    std::vector<std::unique_ptr<IRenderer<prev::render::DefaultRenderContextUserData> > > m_debugRenderers;

    // Shadows
    std::vector<std::unique_ptr<IRenderer<ShadowsRenderContextUserData> > > m_shadowRenderers;

    // Reflection
    std::vector<std::unique_ptr<IRenderer<NormalRenderContextUserData> > > m_reflectionRenderers;

    // Refraction
    std::vector<std::unique_ptr<IRenderer<NormalRenderContextUserData> > > m_refractionRenderers;

#ifdef PARALLEL_RENDERING
    // Parallel stuff
    std::unique_ptr<CommandBuffersGroup> m_defaultCommandBuffersGroup;

    std::unique_ptr<CommandBuffersGroup> m_debugCommandBuffersGroup;

    std::vector<std::unique_ptr<CommandBuffersGroup> > m_shadowsCommandBufferGroups;

    std::unique_ptr<CommandBuffersGroup> m_reflectionCommandBufferGroups;

    std::unique_ptr<CommandBuffersGroup> m_refractionCommandBufferGroups;

    prev::common::ThreadPool m_threadPool{ 8 };
#endif
};

#ifdef PARALLEL_RENDERING
template <typename ContextUserDataType>
void MasterRenderer::RenderParallel(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass, const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode >& root, const std::vector<std::unique_ptr<IRenderer<ContextUserDataType> > >& renderers, const std::vector<VkCommandBuffer>& commandBuffers, const ContextUserDataType& userData, const VkRect2D& area)
{
    renderPass->Begin(renderContext.frameBuffer, renderContext.commandBuffer, area, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

    std::vector<std::future<void> > tasks;
    for (size_t i = 0; i < renderers.size(); i++) {

        auto& renderer = renderers.at(i);
        auto& commandBuffer = commandBuffers.at(i);

        tasks.emplace_back(m_threadPool.Enqueue([&]() {
            VkCommandBufferInheritanceInfo inheritanceInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO };
            inheritanceInfo.renderPass = *renderPass;
            inheritanceInfo.framebuffer = renderContext.frameBuffer;

            VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
            commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
            commandBufferBeginInfo.pInheritanceInfo = &inheritanceInfo;

            VKERRCHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

            prev::render::RenderContext parallelRenderContext{ renderContext.frameBuffer, commandBuffer, renderContext.frameInFlightIndex, renderContext.fullExtent };

            renderer->PreRender(parallelRenderContext, userData);
            renderer->Render(parallelRenderContext, root, userData);
            renderer->PostRender(parallelRenderContext, userData);

            VKERRCHECK(vkEndCommandBuffer(commandBuffer));
        }));
    }

    for (auto&& task : tasks) {
        task.get();
    }

    vkCmdExecuteCommands(renderContext.commandBuffer, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

    renderPass->End(renderContext.commandBuffer);
}
#else
template <typename ContextUserDataType>
void MasterRenderer::RenderSerial(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass, const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode >& root, const std::vector<std::unique_ptr<IRenderer<ContextUserDataType> > >& renderers, const ContextUserDataType& userData, const VkRect2D& area)
{
    renderPass->Begin(renderContext.frameBuffer, renderContext.commandBuffer, area);

    for (auto& renderer : renderers) {
        renderer->PreRender(renderContext, userData);
        renderer->Render(renderContext, root, userData);
        renderer->PostRender(renderContext, userData);
    }

    renderPass->End(renderContext.commandBuffer);
}
#endif

} // namespace prev_test::render::renderer

#endif // !__MASTER_RENDERER_H__
