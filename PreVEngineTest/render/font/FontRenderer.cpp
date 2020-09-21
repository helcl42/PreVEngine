#include "FontRenderer.h"

#include "pipeline/FontPipeline.h"
#include "shader/FonttShader.h"

#include "../../Font.h"

#include <prev/core/DeviceProvider.h>
#include <prev/core/memory/buffer/UniformBuffer.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/AllocatorProvider.h>

#include <memory>

namespace prev_test::render::font {
FontRenderer::FontRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void FontRenderer::Init()
{
    auto device = prev::core::DeviceProvider::Instance().GetDevice();
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    prev::render::shader::ShaderFactory shaderFactory;
    m_shader = shaderFactory.CreateShaderFromFiles<prev_test::render::font::shader::FonttShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, AssetManager::Instance().GetAssetPath("Shaders/font_vert.spv") }, { VK_SHADER_STAGE_FRAGMENT_BIT, AssetManager::Instance().GetAssetPath("Shaders/font_frag.spv") } });
    m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Fonts Shader created\n");

    m_pipeline = std::make_unique<prev_test::render::font::pipeline::FontPipeline>(*device, *m_renderPass, *m_shader);
    m_pipeline->Init();

    LOGI("Fonts Pipeline created\n");

    m_uniformsPoolVS = std::make_unique<prev::core::memory::buffer::UBOPool<UniformsVS> >(*allocator);
    m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

    m_uniformsPoolFS = std::make_unique<prev::core::memory::buffer::UBOPool<UniformsFS> >(*allocator);
    m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
}

void FontRenderer::BeforeRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void FontRenderer::PreRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
    const VkRect2D scissor{ { 0, 0 }, renderContext.fullExtent };
    const VkViewport viewport{ 0, 0, static_cast<float>(renderContext.fullExtent.width), static_cast<float>(renderContext.fullExtent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void FontRenderer::Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData)
{
    if (node->GetFlags().HasAll(prev::common::FlagSet<SceneNodeFlags>{ SceneNodeFlags::FONT_RENDER_COMPONENT })) {
        const auto nodeFontRenderComponent = prev::scene::component::ComponentRepository<IFontRenderComponent>::Instance().Get(node->GetId());
        for (const auto& renderableText : nodeFontRenderComponent->GetRenderableTexts()) {
            auto uboVS = m_uniformsPoolVS->GetNext();
            UniformsVS uniformsVS{};
            uniformsVS.translation = glm::vec4(renderableText.text->GetPosition(), 0.0f, 1.0f);
            uboVS->Update(&uniformsVS);

            auto uboFS = m_uniformsPoolFS->GetNext();
            UniformsFS uniformsFS{};
            uniformsFS.color = renderableText.text->GetColor();
            uniformsFS.width = glm::vec4(renderableText.text->GetWidth());
            uniformsFS.edge = glm::vec4(renderableText.text->GetEdge());
            uniformsFS.bias = glm::vec4(0.0f, 0.0005, 0.0f, 0.0f);
            uniformsFS.borderWidth = glm::vec4(renderableText.text->GetBorderWidth());
            uniformsFS.borderEdge = glm::vec4(renderableText.text->GetBorderEdge());
            uniformsFS.hasEffect = renderableText.text->HasEffect() ? 1 : 0;
            uniformsFS.outlineColor = glm::vec4(renderableText.text->GetOutlineColor(), 1.0f);
            uniformsFS.outlineOffset = glm::vec4(renderableText.text->GetOutlineOffset(), 0.0f, 1.0f);
            uboFS->Update(&uniformsFS);

            m_shader->Bind("textureSampler", *nodeFontRenderComponent->GetFontMetadata()->GetImageBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            m_shader->Bind("uboVS", *uboVS);
            m_shader->Bind("uboFS", *uboFS);

            const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
            const VkBuffer vertexBuffers[] = { *renderableText.model->GetVertexBuffer() };
            const VkDeviceSize offsets[] = { 0 };

            vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.commandBuffer, *renderableText.model->GetIndexBuffer(), 0, renderableText.model->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.commandBuffer, renderableText.model->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
        }
    }

    for (auto child : node->GetChildren()) {
        Render(renderContext, child, renderContextUserData);
    }
}

void FontRenderer::PostRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void FontRenderer::AfterRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void FontRenderer::ShutDown()
{
    m_shader->ShutDown();

    m_pipeline->ShutDown();
}
} // namespace prev_test::render::font