#include "FontRenderer.h"

#include "../../../Tags.h"
#include "../../../common/AssetManager.h"
#include "../../../component/font/IFontRenderComponent.h"
#include "../../../render/font/ScreenSpaceText.h"

#include <prev/common/Common.h>
#include <prev/render/pipeline/PipelineBuilder.h>
#include <prev/render/sampler/SamplerBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::font {
FontRenderer::FontRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene)
    : m_device{ device }
    , m_allocator{ allocator }
    , m_renderPass{ renderPass }
    , m_scene{ scene }
{
}

void FontRenderer::Init()
{
    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/font/font_vert.spv") },
            { VK_SHADER_STAGE_FRAGMENT_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/font/font_frag.spv") }
        })
        .AddVertexInputAttributeDescriptions({
            prev::util::vk::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32_SFLOAT, 0),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC2 }))
        })
        .AddVertexInputBindingDescriptions({
            prev::util::vk::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC2 }), VK_VERTEX_INPUT_RATE_VERTEX)
        })
        .AddDescriptorSets({
            { "uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT },
            { "uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT },
            { "alphaSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT }
        })
	    .SetDescriptorPoolCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Fonts Shader created");

    // clang-format off
    m_pipeline = prev::render::pipeline::GraphicsPipelineBuilder{ m_device, *m_shader, m_renderPass }
        .SetPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetDepthTestEnabled(false)
        .SetDepthWriteEnabled(false)
        .SetBlendingModeEnabled(true)
        .SetAdditiveBlendingEnabled(false)
        .SetPolygonMode(VK_POLYGON_MODE_FILL)
        .SetCullingMode(VK_CULL_MODE_BACK_BIT)
        .Build();
    // clang-format on

    LOGI("Fonts Pipeline created");

    m_uniformsPoolVS = std::make_unique<prev::render::buffer::UniformRingBuffer<UniformsVS>>(m_allocator);
    m_uniformsPoolVS->UpdateCapacity(m_descriptorCount, static_cast<uint32_t>(m_device.GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

    m_uniformsPoolFS = std::make_unique<prev::render::buffer::UniformRingBuffer<UniformsFS>>(m_allocator);
    m_uniformsPoolFS->UpdateCapacity(m_descriptorCount, static_cast<uint32_t>(m_device.GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

    LOGI("Fonts Uniforms Pools created");

    m_alphaSampler = prev::render::sampler::SamplerBuilder{ m_device }
                         .Build();

    LOGI("Fonts Sampler created");
}

void FontRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
}

void FontRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void FontRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (!node->GetTags().HasAll({ TAG_FONT_SCREEN_SPACE_RENDER_COMPONENT })) {
        return;
    }

    const auto nodeFontRenderComponent = prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::font::IFontRenderComponent<prev_test::render::font::ScreenSpaceText>>(node);
    for (const auto& [key, renderableText] : nodeFontRenderComponent->GetRenderableTexts()) {
        auto& uboVS = m_uniformsPoolVS->GetNext();
        UniformsVS uniformsVS{};
        uniformsVS.translation = glm::vec4(renderableText.text->GetPosition(), 0.0f, 1.0f);
        uboVS.Data(uniformsVS);

        auto& uboFS = m_uniformsPoolFS->GetNext();
        UniformsFS uniformsFS{};
        uniformsFS.color = renderableText.text->GetColor();
        uniformsFS.width = glm::vec4(renderableText.text->GetWidth());
        uniformsFS.edge = glm::vec4(renderableText.text->GetEdge());
        uniformsFS.bias = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        uniformsFS.borderWidth = glm::vec4(renderableText.text->GetBorderWidth());
        uniformsFS.borderEdge = glm::vec4(renderableText.text->GetBorderEdge());
        uniformsFS.hasEffect = renderableText.text->HasEffect() ? 1 : 0;
        uniformsFS.outlineColor = glm::vec4(renderableText.text->GetOutlineColor(), 1.0f);
        uniformsFS.outlineOffset = glm::vec4(renderableText.text->GetOutlineOffset(), 0.0f, 1.0f);
        uboFS.Data(uniformsFS);

        m_shader->Bind("alphaSampler", *nodeFontRenderComponent->GetFontMetadata()->GetImageBuffer(), *m_alphaSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        m_shader->Bind("uboVS", uboVS);
        m_shader->Bind("uboFS", uboFS);

        const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
        const VkBuffer vertexBuffers[] = { *renderableText.model->GetVertexBuffer() };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(renderContext.commandBuffer, *renderableText.model->GetIndexBuffer(), 0, renderableText.model->GetIndexBuffer()->GetIndexType());
        vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(renderContext.commandBuffer, renderableText.model->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
    }
}

void FontRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void FontRenderer::AfterRender(const NormalRenderContext& renderContext)
{
}

void FontRenderer::ShutDown()
{
    m_alphaSampler = {};

    m_uniformsPoolFS = {};
    m_uniformsPoolVS = {};

    m_pipeline = {};
    m_shader = {};
}
} // namespace prev_test::render::renderer::font
