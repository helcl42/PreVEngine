#include "FontRenderer.h"

#include "../../../Tags.h"
#include "../../../common/ShaderAssetManager.h"
#include "../../../component/font/IFontRenderComponent.h"
#include "../../../render/font/ScreenSpaceText.h"

#include <prev/render/buffer/BufferPoolBuilder.h>
#include <prev/render/pipeline/GraphicsPipelineBuilder.h>
#include <prev/render/sampler/SamplerBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::font {
FontRenderer::FontRenderer(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene)
    : m_device{ device }
    , m_renderPass{ renderPass }
    , m_scene{ scene }
{
}

void FontRenderer::Init()
{
    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { GFX_SHADER_STAGE_VERTEX, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "font/font_vert") },
            { GFX_SHADER_STAGE_FRAGMENT, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "font/font_frag") }
        })
        .AddVertexInputAttributes({
            prev::render::shader::VertexInputAttribute{ 0, 0, GFX_FORMAT_R32G32_FLOAT, 0 },
            prev::render::shader::VertexInputAttribute{ 0, 1, GFX_FORMAT_R32G32_FLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC2 })}
        })
        .AddVertexInputBindings({
            prev::render::shader::VertexInputBinding{ 0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC2 }), GFX_VERTEX_STEP_MODE_VERTEX }
        })
        .AddDescriptorSets({
            { "uboVS", 0, GFX_BINDING_TYPE_BUFFER, GFX_SHADER_STAGE_VERTEX },
            { "uboFS", 1, GFX_BINDING_TYPE_BUFFER, GFX_SHADER_STAGE_FRAGMENT },
            { "alphaTexture", 2, GFX_BINDING_TYPE_TEXTURE, GFX_SHADER_STAGE_FRAGMENT },
            { "alphaSampler", 3, GFX_BINDING_TYPE_SAMPLER, GFX_SHADER_STAGE_FRAGMENT }
        })
	    .SetBindGroupCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Fonts Shader created");

    // clang-format off
    m_pipeline = prev::render::pipeline::GraphicsPipelineBuilder{ m_device, *m_shader, m_renderPass }
        .SetPrimitiveTopology(GFX_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetDepthTestEnabled(false)
        .SetDepthWriteEnabled(false)
        .SetBlendingModeEnabled(true)
        .SetAdditiveBlendingEnabled(false)
        .SetPolygonMode(GFX_POLYGON_MODE_FILL)
        .SetCullingMode(GFX_CULL_MODE_BACK)
        .Build();
    // clang-format on

    LOGI("Fonts Pipeline created");

    m_uniformsPoolVS = prev::render::buffer::BufferPoolBuilder{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                           .SetHostMapped(true)
                           .SetUsageFlags(GFX_BUFFER_USAGE_UNIFORM)
                           .SetCount(m_descriptorCount)
                           .SetStride(sizeof(UniformsVS))
                           .SetAlignment(m_device.GetGPU().GetLimits().minUniformBufferOffsetAlignment)
                           .Build();

    m_uniformsPoolFS = prev::render::buffer::BufferPoolBuilder{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                           .SetHostMapped(true)
                           .SetUsageFlags(GFX_BUFFER_USAGE_UNIFORM)
                           .SetCount(m_descriptorCount)
                           .SetStride(sizeof(UniformsFS))
                           .SetAlignment(m_device.GetGPU().GetLimits().minUniformBufferOffsetAlignment)
                           .Build();

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
    const GfxViewport viewport{ static_cast<float>(renderContext.rect.origin.x), static_cast<float>(renderContext.rect.origin.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0.0f, 1.0f };

    gfxRenderPassEncoderSetPipeline(renderContext.renderPassEncoder, *m_pipeline);
    gfxRenderPassEncoderSetViewport(renderContext.renderPassEncoder, &viewport);
    gfxRenderPassEncoderSetScissorRect(renderContext.renderPassEncoder, &renderContext.rect);
}

void FontRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (!node->GetTags().HasAll({ TAG_FONT_SCREEN_SPACE_RENDER_COMPONENT })) {
        return;
    }

    const auto nodeFontRenderComponent = prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::font::IFontRenderComponent<prev_test::render::font::ScreenSpaceText>>(node);
    for (const auto& [key, renderableText] : nodeFontRenderComponent->GetRenderableTexts()) {
        m_uniformsPoolVS->MoveToNext();

        auto& uboVS = m_uniformsPoolVS->GetCurrent();

        UniformsVS uniformsVS{};
        uniformsVS.translation = glm::vec4(renderableText.text->GetPosition(), 0.0f, 1.0f);
        uboVS.Write(uniformsVS);

        m_uniformsPoolFS->MoveToNext();

        auto& uboFS = m_uniformsPoolFS->GetCurrent();

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
        uboFS.Write(uniformsFS);

        m_shader->Bind("alphaTexture", nodeFontRenderComponent->GetFontMetadata()->GetImageBuffer()->GetTextureView());
        m_shader->Bind("alphaSampler", *m_alphaSampler);
        m_shader->Bind("uboVS", uboVS);
        m_shader->Bind("uboFS", uboFS);

        const GfxBindGroup descriptorSet = m_shader->UpdateNextBindGroup();
        const uint64_t vertexOffset = 0;
        const uint64_t vertexRange = renderableText.model->GetVertexBuffer()->GetSize() - vertexOffset;
        gfxRenderPassEncoderSetVertexBuffer(renderContext.renderPassEncoder, 0, *renderableText.model->GetVertexBuffer(), vertexOffset, vertexRange);
        gfxRenderPassEncoderSetIndexBuffer(renderContext.renderPassEncoder, *renderableText.model->GetIndexBuffer(), GFX_INDEX_FORMAT_UINT32, 0, renderableText.model->GetIndexBuffer()->GetSize());
        gfxRenderPassEncoderSetBindGroup(renderContext.renderPassEncoder, 0, descriptorSet, nullptr, 0);

        gfxRenderPassEncoderDrawIndexed(renderContext.renderPassEncoder, renderableText.model->GetMesh()->GetIndicesCount(), 1, 0, 0, 0);
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
