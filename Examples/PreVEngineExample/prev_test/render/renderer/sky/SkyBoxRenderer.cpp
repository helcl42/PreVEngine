#include "SkyBoxRenderer.h"

#include "../../../Tags.h"
#include "../../../common/ShaderAssetManager.h"
#include "../../../component/sky/ISkyBoxComponent.h"
#include "../../../component/sky/SkyCommon.h"
#include "../../../component/transform/ITransformComponent.h"

#include <prev/render/buffer/BufferPoolBuilder.h>
#include <prev/render/pipeline/GraphicsPipelineBuilder.h>
#include <prev/render/sampler/SamplerBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::sky {
SkyBoxRenderer::SkyBoxRenderer(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene)
    : m_device{ device }
    , m_renderPass{ renderPass }
    , m_scene{ scene }
{
}

void SkyBoxRenderer::Init()
{
    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { GFX_SHADER_STAGE_VERTEX, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "sky/skybox_vert") },
            { GFX_SHADER_STAGE_FRAGMENT, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "sky/skybox_frag") }
        })
        .AddVertexInputAttributes({
            prev::render::shader::VertexInputAttribute{ 0, 0, GFX_FORMAT_R32G32B32_FLOAT, 0 },
            prev::render::shader::VertexInputAttribute{ 0, 1, GFX_FORMAT_R32G32_FLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })},
            prev::render::shader::VertexInputAttribute{ 0, 2, GFX_FORMAT_R32G32B32_FLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })}
        })
        .AddVertexInputBindings({
            prev::render::shader::VertexInputBinding{ 0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), GFX_VERTEX_STEP_MODE_VERTEX }
        })
        .AddDescriptorSets({
            prev::render::shader::ShaderBuilder::DescriptorSet::Buffer("uboVS", 0, GFX_SHADER_STAGE_VERTEX),
            prev::render::shader::ShaderBuilder::DescriptorSet::Buffer("uboFS", 1, GFX_SHADER_STAGE_FRAGMENT),
            prev::render::shader::ShaderBuilder::DescriptorSet::Texture("cubeMap1Texture", 2, GFX_SHADER_STAGE_FRAGMENT, GFX_TEXTURE_VIEW_TYPE_CUBE),
            prev::render::shader::ShaderBuilder::DescriptorSet::Sampler("cubeMap1Sampler", 3, GFX_SHADER_STAGE_FRAGMENT)
        })
        .SetBindGroupCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Skybox Shader created");

    // clang-format off
    m_pipeline = prev::render::pipeline::GraphicsPipelineBuilder{ m_device, *m_shader, m_renderPass }
        .SetPrimitiveTopology(GFX_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetDepthTestEnabled(false)
        .SetDepthWriteEnabled(false)
        .SetBlendingModeEnabled(false)
        .SetAdditiveBlendingEnabled(false)
        .SetPolygonMode(GFX_POLYGON_MODE_FILL)
        .SetCullingMode(GFX_CULL_MODE_FRONT)
        .Build();
    // clang-format on

    LOGI("Skybox Pipeline created");

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

    LOGI("Skybox Uniforms Pools created");

    m_colorSampler = prev::render::sampler::SamplerBuilder{ m_device }
                         .Build();

    LOGI("Skybox Sampler created");
}

void SkyBoxRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
}

void SkyBoxRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const GfxViewport viewport{ static_cast<float>(renderContext.rect.origin.x), static_cast<float>(renderContext.rect.origin.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0.0f, 1.0f };

    gfxRenderPassEncoderSetPipeline(renderContext.renderPassEncoder, *m_pipeline);
    gfxRenderPassEncoderSetViewport(renderContext.renderPassEncoder, &viewport);
    gfxRenderPassEncoderSetScissorRect(renderContext.renderPassEncoder, &renderContext.rect);
}

void SkyBoxRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (!node->GetTags().HasAll({ TAG_SKYBOX_RENDER_COMPONENT })) {
        return;
    }

    if (!node->GetTags().HasAll({ TAG_TRANSFORM_COMPONENT })) {
        return;
    }

    const auto transformComponent = prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::transform::ITransformComponent>(node);
    const auto skyBoxComponent = prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::sky::ISkyBoxComponent>(node);

    m_uniformsPoolVS->MoveToNext();

    auto& uboVS = m_uniformsPoolVS->GetCurrent();

    UniformsVS uniformsVS{};
    uniformsVS.modelMatrix = transformComponent->GetWorldTransformScaled();
    for (uint32_t i = 0; i < renderContext.cameraCount; ++i) {
        uniformsVS.viewMatrices[i] = renderContext.viewMatrices[i];
        uniformsVS.projectionMatrices[i] = renderContext.projectionMatrices[i];
    }
    uboVS.Write(uniformsVS);

    m_uniformsPoolFS->MoveToNext();

    auto& uboFS = m_uniformsPoolFS->GetCurrent();

    UniformsFS uniformsFS{};
    uniformsFS.fogColor = prev_test::component::sky::FOG_COLOR;
    uniformsFS.lowerLimit = glm::vec4(0.0f);
    uniformsFS.upperLimit = glm::vec4(0.03f);
    uboFS.Write(uniformsFS);

    m_shader->Bind("cubeMap1Texture", skyBoxComponent->GetMaterial()->GetImageBuffer()->GetTextureView());
    m_shader->Bind("cubeMap1Sampler", *m_colorSampler);
    m_shader->Bind("uboVS", uboVS);
    m_shader->Bind("uboFS", uboFS);

    const GfxBindGroup descriptorSet = m_shader->UpdateNextBindGroup();
    const uint64_t vertexOffset = 0;
    const uint64_t vertexRange = skyBoxComponent->GetModel()->GetVertexBuffer()->GetSize() - vertexOffset;
    gfxRenderPassEncoderSetVertexBuffer(renderContext.renderPassEncoder, 0, *skyBoxComponent->GetModel()->GetVertexBuffer(), vertexOffset, vertexRange);
    gfxRenderPassEncoderSetIndexBuffer(renderContext.renderPassEncoder, *skyBoxComponent->GetModel()->GetIndexBuffer(), GFX_INDEX_FORMAT_UINT32, 0, skyBoxComponent->GetModel()->GetIndexBuffer()->GetSize());
    gfxRenderPassEncoderSetBindGroup(renderContext.renderPassEncoder, 0, descriptorSet, nullptr, 0);

    gfxRenderPassEncoderDrawIndexed(renderContext.renderPassEncoder, skyBoxComponent->GetModel()->GetMesh()->GetIndicesCount(), 1, 0, 0, 0);
}

void SkyBoxRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void SkyBoxRenderer::AfterRender(const NormalRenderContext& renderContext)
{
}

void SkyBoxRenderer::ShutDown()
{
    m_colorSampler = {};

    m_uniformsPoolFS = {};
    m_uniformsPoolVS = {};

    m_pipeline = {};
    m_shader = {};
}
} // namespace prev_test::render::renderer::sky
