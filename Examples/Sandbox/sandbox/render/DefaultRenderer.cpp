#include "DefaultRenderer.h"

#include "../Tags.h"
#include "../common/ShaderPath.h"
#include "../component/ColorComponent.h"
#include "../component/ModelComponent.h"
#include "../component/TransformComponent.h"
#include "Model.h"

#include <prev/render/buffer/BufferPoolBuilder.h>
#include <prev/render/pipeline/GraphicsPipelineBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/NodeComponentHelper.h>

#include <glm/gtc/matrix_transform.hpp>

namespace sandbox::render {
DefaultRenderer::DefaultRenderer(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass)
    : m_device{ device }
    , m_renderPass{ renderPass }
{
}

void DefaultRenderer::Init()
{
    const GfxBackend backend{ m_device.GetGPU().GetInfo().backend };

    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { GFX_SHADER_STAGE_VERTEX, sandbox::common::ShaderPath(backend, "sandbox/sandbox_vert") },
            { GFX_SHADER_STAGE_FRAGMENT, sandbox::common::ShaderPath(backend, "sandbox/sandbox_frag") }
        })
        .AddVertexInputAttributes({
            prev::render::shader::VertexInputAttribute{ 0, 0, GFX_FORMAT_R32G32B32_FLOAT, 0 },
            prev::render::shader::VertexInputAttribute{ 0, 1, GFX_FORMAT_R32G32B32_FLOAT, sizeof(glm::vec3) }
        })
        .AddVertexInputBindings({
            prev::render::shader::VertexInputBinding{ 0, sizeof(sandbox::render::Vertex), GFX_VERTEX_STEP_MODE_VERTEX }
        })
        .AddBindGroupEntries({
            prev::render::shader::ShaderBuilder::BindGroupEntry::Buffer("ubo", 0, GFX_SHADER_STAGE_VERTEX | GFX_SHADER_STAGE_FRAGMENT)
        })
        .Build();

    m_pipeline = prev::render::pipeline::GraphicsPipelineBuilder{ m_device, *m_shader, m_renderPass }
        .SetPrimitiveTopology(GFX_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetDepthTestEnabled(true)
        .SetDepthWriteEnabled(true)
        .SetBlendingModeEnabled(false)
        .SetAdditiveBlendingEnabled(false)
        .SetPolygonMode(GFX_POLYGON_MODE_FILL)
        .SetCullingMode(GFX_CULL_MODE_BACK)
        .Build();
    // clang-format on

    m_uniformsPool = prev::render::buffer::BufferPoolBuilder{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                         .SetMemoryProperties(GFX_MEMORY_PROPERTY_HOST_VISIBLE | GFX_MEMORY_PROPERTY_HOST_COHERENT)
                         .SetUsageFlags(GFX_BUFFER_USAGE_UNIFORM | GFX_BUFFER_USAGE_MAP_WRITE)
                         .SetChunkSize(m_uniformPoolChunk)
                         .SetStride(sizeof(SandboxUniforms))
                         .SetAlignment(m_device.GetGPU().GetLimits().minUniformBufferOffsetAlignment)
                         .BuildFrameScoped();
}

void DefaultRenderer::BeginFrame(uint32_t frameInFlightIndex)
{
    m_shader->BeginFrame(frameInFlightIndex);
    m_uniformsPool->BeginFrame(frameInFlightIndex);
}

void DefaultRenderer::EndFrame()
{
    m_shader->EndFrame();
    m_uniformsPool->EndFrame();
}

void DefaultRenderer::Render(GfxRenderPassEncoder encoder, const GfxScissorRect& rect, const ViewData& views, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (!node->GetTags().HasAll({ sandbox::TAG_RENDERABLE })) {
        return;
    }

    const auto transformComponent{ prev::scene::component::NodeComponentHelper::GetComponent<sandbox::component::TransformComponent>(node) };
    const auto colorComponent{ prev::scene::component::NodeComponentHelper::GetComponent<sandbox::component::ColorComponent>(node) };
    const auto modelComponent{ prev::scene::component::NodeComponentHelper::GetComponent<sandbox::component::ModelComponent>(node) };

    const glm::mat4 modelMatrix{ transformComponent->GetWorldTransform() };

    const GfxViewport viewport{ static_cast<float>(rect.origin.x), static_cast<float>(rect.origin.y), static_cast<float>(rect.extent.width), static_cast<float>(rect.extent.height), 0.0f, 1.0f };
    gfxRenderPassEncoderSetPipeline(encoder, *m_pipeline);
    gfxRenderPassEncoderSetViewport(encoder, &viewport);
    gfxRenderPassEncoderSetScissorRect(encoder, &rect);

    auto& ubo{ m_uniformsPool->Next() };

    SandboxUniforms uniforms{};
    uniforms.modelMatrix = modelMatrix;
    uniforms.normalMatrix = modelMatrix;
    for (uint32_t view = 0; view < views.viewCount; ++view) {
        uniforms.viewMatrices[view] = views.viewMatrices[view];
        uniforms.projectionMatrices[view] = views.projectionMatrices[view];
    }
    uniforms.lightDirection = glm::vec4(glm::normalize(glm::vec3{ 0.5f, 1.0f, 0.4f }), 0.0f);
    uniforms.color = colorComponent->GetColor();
    ubo.Write(uniforms);

    m_shader->Bind("ubo", ubo);
    const GfxBindGroup bindGroup{ m_shader->UpdateNextBindGroup() };

    const auto& model{ modelComponent->GetModel() };

    gfxRenderPassEncoderSetVertexBuffer(encoder, 0, *model->vertexBuffer, 0, model->vertexBuffer->GetSize());
    gfxRenderPassEncoderSetIndexBuffer(encoder, *model->indexBuffer, GFX_INDEX_FORMAT_UINT32, 0, model->indexBuffer->GetSize());
    gfxRenderPassEncoderSetBindGroup(encoder, 0, bindGroup, nullptr, 0);
    gfxRenderPassEncoderDrawIndexed(encoder, model->indexCount, 1, 0, 0, 0);
}

void DefaultRenderer::ShutDown()
{
    m_uniformsPool.reset();
    m_pipeline.reset();
    m_shader.reset();
}
} // namespace sandbox::render
