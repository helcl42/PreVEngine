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
        .AddBindGroupEntries({
            prev::render::shader::ShaderBuilder::BindGroupEntry::Buffer("uboVS", 0, GFX_SHADER_STAGE_VERTEX),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Buffer("uboFS", 1, GFX_SHADER_STAGE_FRAGMENT),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Texture("cubeMap1Texture", 2, GFX_SHADER_STAGE_FRAGMENT, GFX_TEXTURE_VIEW_TYPE_CUBE),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Sampler("cubeMap1Sampler", 3, GFX_SHADER_STAGE_FRAGMENT)
        })
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
                           .SetMemoryProperties(GFX_MEMORY_PROPERTY_HOST_VISIBLE | GFX_MEMORY_PROPERTY_HOST_COHERENT)
                           .SetUsageFlags(GFX_BUFFER_USAGE_UNIFORM | GFX_BUFFER_USAGE_MAP_WRITE)
                           .SetChunkSize(m_descriptorCount)
                           .SetStride(sizeof(UniformsVS))
                           .SetAlignment(m_device.GetGPU().GetLimits().minUniformBufferOffsetAlignment)
                           .BuildFrameScoped();

    m_uniformsPoolFS = prev::render::buffer::BufferPoolBuilder{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                           .SetMemoryProperties(GFX_MEMORY_PROPERTY_HOST_VISIBLE | GFX_MEMORY_PROPERTY_HOST_COHERENT)
                           .SetUsageFlags(GFX_BUFFER_USAGE_UNIFORM | GFX_BUFFER_USAGE_MAP_WRITE)
                           .SetChunkSize(m_descriptorCount)
                           .SetStride(sizeof(UniformsFS))
                           .SetAlignment(m_device.GetGPU().GetLimits().minUniformBufferOffsetAlignment)
                           .BuildFrameScoped();

    LOGI("Skybox Uniforms Pools created");

    m_colorSampler = prev::render::sampler::SamplerBuilder{ m_device }
                         .Build();

    LOGI("Skybox Sampler created");
}

void SkyBoxRenderer::BeginFrame(const NormalRenderContext& renderContext)
{
    m_shader->BeginFrame(renderContext.frameInFlightIndex);
    m_uniformsPoolVS->BeginFrame(renderContext.frameInFlightIndex);
    m_uniformsPoolFS->BeginFrame(renderContext.frameInFlightIndex);
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
    if (!skyBoxComponent->IsReady()) {
        return;
    }

    auto& uboVS = m_uniformsPoolVS->Next();

    UniformsVS uniformsVS{};
    uniformsVS.modelMatrix = transformComponent->GetWorldTransformScaled();
    for (uint32_t i = 0; i < renderContext.cameraCount; ++i) {
        uniformsVS.viewMatrices[i] = renderContext.viewMatrices[i];
        uniformsVS.projectionMatrices[i] = renderContext.projectionMatrices[i];
    }
    uboVS.Write(uniformsVS);

    auto& uboFS = m_uniformsPoolFS->Next();

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

void SkyBoxRenderer::EndFrame(const NormalRenderContext& renderContext)
{
    m_shader->EndFrame();
    m_uniformsPoolVS->EndFrame();
    m_uniformsPoolFS->EndFrame();
}

void SkyBoxRenderer::ShutDown()
{
    m_colorSampler.reset();

    m_uniformsPoolFS.reset();
    m_uniformsPoolVS.reset();

    m_pipeline.reset();
    m_shader.reset();
}
} // namespace prev_test::render::renderer::sky
