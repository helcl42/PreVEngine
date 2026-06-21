#include "SunRenderer.h"
#include "SkyEvents.h"

#include "../../../Tags.h"
#include "../../../common/ShaderAssetManager.h"
#include "../../../component/light/ILightComponent.h"
#include "../../../component/sky/ISunComponent.h"

#include <prev/event/EventChannel.h>
#include <prev/render/buffer/BufferPoolBuilder.h>
#include <prev/render/pipeline/GraphicsPipelineBuilder.h>
#include <prev/render/query/QueryPoolBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::sky {
SunRenderer::SunRenderer(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene)
    : m_device{ device }
    , m_renderPass{ renderPass }
    , m_scene{ scene }
{
}

void SunRenderer::Init()
{
    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { GFX_SHADER_STAGE_VERTEX, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "sky/sun_occlusion_vert") },
            { GFX_SHADER_STAGE_FRAGMENT, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "sky/sun_occlusion_frag") }
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
            prev::render::shader::ShaderBuilder::BindGroupEntry::Buffer("uboVS", 0, GFX_SHADER_STAGE_VERTEX)
        })
        .Build();
    // clang-format on

    LOGI("Sun Shader created");

    // clang-format off
    m_pipeline = prev::render::pipeline::GraphicsPipelineBuilder{ m_device, *m_shader, m_renderPass }
        .SetPrimitiveTopology(GFX_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetDepthTestEnabled(true)
        .SetDepthWriteEnabled(true)
        .SetBlendingModeEnabled(true)
        .SetAdditiveBlendingEnabled(false)
        .SetPolygonMode(GFX_POLYGON_MODE_FILL)
        .SetCullingMode(GFX_CULL_MODE_NONE)
        .Build();
    // clang-format on

    LOGI("Sun Pipeline created");

    m_uniformsPoolVS = prev::render::buffer::BufferPoolBuilder{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                           .SetMemoryProperties(GFX_MEMORY_PROPERTY_HOST_VISIBLE | GFX_MEMORY_PROPERTY_HOST_COHERENT)
                           .SetUsageFlags(GFX_BUFFER_USAGE_UNIFORM | GFX_BUFFER_USAGE_MAP_WRITE)
                           .SetChunkSize(m_descriptorCount)
                           .SetStride(sizeof(UniformsVS))
                           .SetAlignment(m_device.GetGPU().GetLimits().minUniformBufferOffsetAlignment)
                           .BuildFrameScoped();

    LOGI("Sun Uniforms Pools created");

    m_queryPool = prev::render::query::QueryPoolBuilder{ m_device }
                      .SetQueryType(GFX_QUERY_TYPE_OCCLUSION)
                      .SetPoolCount(QueryPoolCount)
                      .SetQueryCount(MAX_VIEW_COUNT)
                      .SetPrecise(m_device.HasExtension(GFX_DEVICE_EXTENSION_OCCLUSION_QUERY_PRECISE))
                      .Build();
    m_passedSamples = 0;

    LOGI("Sun Query Pool created");
}

void SunRenderer::BeginFrame(const NormalRenderContext& renderContext)
{
    m_shader->BeginFrame(renderContext.frameInFlightIndex);
    m_uniformsPoolVS->BeginFrame(renderContext.frameInFlightIndex);

    m_renderPass.SetOcclusionQuerySet(*m_queryPool);

    if (m_queryPool->IsAsyncResultReady()) {
        if (m_queryPool->GetAsyncQueryResult<uint64_t>(0, m_passedSamples)) {
            float ratio;
            if (m_device.HasExtension(GFX_DEVICE_EXTENSION_OCCLUSION_QUERY_PRECISE)) {
                ratio = glm::clamp((static_cast<float>(m_passedSamples) / static_cast<float>(m_maxNumberOfSamples * m_renderPass.GetSampleCount())) * 0.5f, 0.0f, 1.0f);
            } else {
                ratio = m_passedSamples > 0 ? 0.5f : 0.0f;
            }
            prev::event::EventChannel::Post(prev_test::render::renderer::sky::SunVisibilityEvent{ ratio });
        }
    }

    m_queryPool->StartAsyncMapRead();
}

void SunRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const GfxViewport viewport{ static_cast<float>(renderContext.rect.origin.x), static_cast<float>(renderContext.rect.origin.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0.0f, 1.0f };

    gfxRenderPassEncoderSetPipeline(renderContext.renderPassEncoder, *m_pipeline);
    gfxRenderPassEncoderSetViewport(renderContext.renderPassEncoder, &viewport);
    gfxRenderPassEncoderSetScissorRect(renderContext.renderPassEncoder, &renderContext.rect);
}

void SunRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (!node->GetTags().HasAll({ TAG_SUN_RENDER_COMPONENT })) {
        return;
    }

    const auto sunComponent{ prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::sky::ISunComponent>(node) };
    const auto lightComponent{ prev::scene::component::NodeComponentHelper::Find<prev_test::component::light::ILightComponent>(m_scene.GetRootNode(), { TAG_MAIN_LIGHT }) };

    const float aspectRatio{
        static_cast<float>(renderContext.rect.extent.width - renderContext.rect.origin.x) / static_cast<float>(renderContext.rect.extent.height - renderContext.rect.origin.y)
    };
    const float xScale{ sunComponent->GetFlare().GetScale() };
    const float yScale{ xScale * aspectRatio * (m_device.GetGPU().GetInfo().backend == GFX_BACKEND_WEBGPU ? -1.0f : 1.0f) };

    m_maxNumberOfSamples = static_cast<uint64_t>(xScale * static_cast<float>(renderContext.rect.extent.width - renderContext.rect.origin.x) * yScale * static_cast<float>(renderContext.rect.extent.height - renderContext.rect.origin.y));
    m_queryPool->BeginQuery(0, renderContext.renderPassEncoder);

    auto& uboVS = m_uniformsPoolVS->Next();
    UniformsVS uniformsVS{};
    for (uint32_t viewIndex = 0; viewIndex < renderContext.cameraCount; ++viewIndex) {
        const auto sunPosition{ sunComponent->ComputeFlarePosition(renderContext.projectionMatrices[viewIndex], renderContext.viewMatrices[viewIndex], renderContext.cameraPositions[viewIndex], lightComponent->GetPosition()) };
        uniformsVS.translations[viewIndex] = glm::vec4(sunPosition, MAX_DEPTH - 0.0001f, 1.0f);
    }
    uniformsVS.scale = glm::vec4(xScale, yScale, 0.0f, 0.0f);
    uboVS.Write(uniformsVS);

    m_shader->Bind("uboVS", uboVS);

    const GfxBindGroup descriptorSet = m_shader->UpdateNextBindGroup();
    const uint64_t vertexOffset = 0;
    const uint64_t vertexRange = sunComponent->GetModel()->GetVertexBuffer()->GetSize() - vertexOffset;
    gfxRenderPassEncoderSetVertexBuffer(renderContext.renderPassEncoder, 0, *sunComponent->GetModel()->GetVertexBuffer(), vertexOffset, vertexRange);
    gfxRenderPassEncoderSetIndexBuffer(renderContext.renderPassEncoder, *sunComponent->GetModel()->GetIndexBuffer(), GFX_INDEX_FORMAT_UINT32, 0, sunComponent->GetModel()->GetIndexBuffer()->GetSize());
    gfxRenderPassEncoderSetBindGroup(renderContext.renderPassEncoder, 0, descriptorSet, nullptr, 0);

    gfxRenderPassEncoderDrawIndexed(renderContext.renderPassEncoder, sunComponent->GetModel()->GetMesh()->GetIndicesCount(), 1, 0, 0, 0);

    m_queryPool->EndQuery(0, renderContext.renderPassEncoder);
}

void SunRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void SunRenderer::EndFrame(const NormalRenderContext& renderContext)
{
    m_queryPool->Resolve(renderContext.commandEncoder);

    m_shader->EndFrame();
    m_uniformsPoolVS->EndFrame();
}

void SunRenderer::ShutDown()
{
    m_renderPass.SetOcclusionQuerySet(nullptr);

    m_queryPool.reset();

    m_uniformsPoolVS.reset();

    m_pipeline.reset();
    m_shader.reset();
}
} // namespace prev_test::render::renderer::sky
