#include "HandTrackingRenderer.h"

#ifdef ENABLE_XR

#include "../../../Tags.h"
#include "../../../common/ShaderAssetManager.h"
#include "../../../component/hand_tracking/IHandTrackingComponent.h"
#include "../../VertexLayout.h"

#include <prev/render/buffer/BufferPoolBuilder.h>
#include <prev/render/pipeline/GraphicsPipelineBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::xr {
HandTrackingRenderer::HandTrackingRenderer(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene)
    : m_device{ device }
    , m_renderPass{ renderPass }
    , m_scene{ scene }
{
}

void HandTrackingRenderer::Init()
{
    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { GFX_SHADER_STAGE_VERTEX, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "xr/hand_tracking_vert") },
            { GFX_SHADER_STAGE_FRAGMENT, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "xr/hand_tracking_frag") }
        })
        .AddVertexInputAttributes({
            prev::render::shader::VertexInputAttribute{ 0, 0, GFX_FORMAT_R32G32B32_FLOAT, 0 },
            prev::render::shader::VertexInputAttribute{ 0, 1, GFX_FORMAT_R32G32_FLOAT, prev_test::render::VertexLayout::GetComponentsSize({ prev_test::render::VertexLayoutComponent::VEC3 })},
            prev::render::shader::VertexInputAttribute{ 0, 2, GFX_FORMAT_R32G32B32_FLOAT, prev_test::render::VertexLayout::GetComponentsSize({ prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC2 })}
        })
        .AddVertexInputBindings({
            prev::render::shader::VertexInputBinding{ 0, prev_test::render::VertexLayout::GetComponentsSize({ prev_test::render::VertexLayoutComponent::VEC3, prev_test::render::VertexLayoutComponent::VEC2, prev_test::render::VertexLayoutComponent::VEC3 }), GFX_VERTEX_STEP_MODE_VERTEX }
        })
        .AddBindGroupEntries({
            prev::render::shader::ShaderBuilder::BindGroupEntry::Buffer("uboVS", 0, GFX_SHADER_STAGE_VERTEX),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Buffer("uboFS", 1, GFX_SHADER_STAGE_FRAGMENT)
        })
        .Build();
    // clang-format on

    LOGI("Hand Tracking Shader created");

    // clang-format off
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

    LOGI("Hand Tracking Pipeline created");

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

    LOGI("Hand Tracking Uniforms Pools created");
}

void HandTrackingRenderer::BeginFrame(const NormalRenderContext& renderContext)
{
    m_shader->BeginFrame(renderContext.frameInFlightIndex);
    m_uniformsPoolVS->BeginFrame(renderContext.frameInFlightIndex);
    m_uniformsPoolFS->BeginFrame(renderContext.frameInFlightIndex);
}

void HandTrackingRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const GfxViewport viewport{ static_cast<float>(renderContext.rect.origin.x), static_cast<float>(renderContext.rect.origin.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0.0f, 1.0f };

    gfxRenderPassEncoderSetPipeline(renderContext.renderPassEncoder, *m_pipeline);
    gfxRenderPassEncoderSetViewport(renderContext.renderPassEncoder, &viewport);
    gfxRenderPassEncoderSetScissorRect(renderContext.renderPassEncoder, &renderContext.rect);
}

void HandTrackingRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (!node->GetTags().HasAll({ TAG_HAND_TRACKING_RENDER_COMPONENT })) {
        return;
    }

    const auto handTrackingComponent = prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::hand_tracking::IHandTrackingComponent>(node);
    const auto& handsData = handTrackingComponent->GetHandsData();
    const auto jointModel = handTrackingComponent->GetJointModel();

    // Colors per hand: left = cyan, right = magenta
    static const glm::vec4 HAND_COLORS[prev::xr::MAX_HAND_COUNT] = {
        glm::vec4(0.0f, 0.8f, 0.9f, 1.0f), // left - cyan
        glm::vec4(0.9f, 0.2f, 0.8f, 1.0f) // right - magenta
    };

    for (uint32_t handIdx = 0; handIdx < prev::xr::MAX_HAND_COUNT; ++handIdx) {
        const auto& hand = handsData.hands[handIdx];
        if (!hand.active) {
            continue;
        }

        for (uint32_t jointIdx = 0; jointIdx < prev::xr::MAX_HAND_TRACKING_JOINT_COUNT; ++jointIdx) {
            const auto& joint = hand.joints[jointIdx];
            if (!joint.active) {
                continue;
            }

            // Scale cube by joint radius
            const float scale = joint.radius > 0.0f ? joint.radius : 0.005f;
            const glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), joint.pose.position);
            const glm::mat4 rotationMatrix = glm::mat4_cast(joint.pose.orientation);
            const glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
            const glm::mat4 modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;

            auto& uboVS = m_uniformsPoolVS->Next();

            UniformsVS uniformsVS{};
            uniformsVS.modelMatrix = modelMatrix;
            for (uint32_t i = 0; i < renderContext.cameraCount; ++i) {
                uniformsVS.viewMatrices[i] = renderContext.viewMatrices[i];
                uniformsVS.projectionMatrices[i] = renderContext.projectionMatrices[i];
            }
            uboVS.Write(uniformsVS);

            auto& uboFS = m_uniformsPoolFS->Next();

            UniformsFS uniformsFS{};
            uniformsFS.color = HAND_COLORS[handIdx];
            uboFS.Write(uniformsFS);

            m_shader->Bind("uboVS", uboVS);
            m_shader->Bind("uboFS", uboFS);

            const GfxBindGroup descriptorSet = m_shader->UpdateNextBindGroup();
            const uint64_t vertexOffset = 0;
            const uint64_t vertexRange = jointModel->GetVertexBuffer()->GetSize() - vertexOffset;
            gfxRenderPassEncoderSetVertexBuffer(renderContext.renderPassEncoder, 0, *jointModel->GetVertexBuffer(), vertexOffset, vertexRange);
            gfxRenderPassEncoderSetIndexBuffer(renderContext.renderPassEncoder, *jointModel->GetIndexBuffer(), GFX_INDEX_FORMAT_UINT32, 0, jointModel->GetIndexBuffer()->GetSize());
            gfxRenderPassEncoderSetBindGroup(renderContext.renderPassEncoder, 0, descriptorSet, nullptr, 0);

            gfxRenderPassEncoderDrawIndexed(renderContext.renderPassEncoder, jointModel->GetMesh()->GetIndicesCount(), 1, 0, 0, 0);
        }
    }
}

void HandTrackingRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void HandTrackingRenderer::EndFrame(const NormalRenderContext& renderContext)
{
    m_shader->EndFrame();
    m_uniformsPoolVS->EndFrame();
    m_uniformsPoolFS->EndFrame();
}

void HandTrackingRenderer::ShutDown()
{
    m_uniformsPoolFS.reset();
    m_uniformsPoolVS.reset();

    m_pipeline.reset();
    m_shader.reset();
}
} // namespace prev_test::render::renderer::xr

#endif
