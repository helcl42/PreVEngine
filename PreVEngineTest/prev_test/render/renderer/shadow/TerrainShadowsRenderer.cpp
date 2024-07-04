#include "TerrainShadowsRenderer.h"
#include "pipeline/TerrainShadowsPipeline.h"

#include "../../../common/AssetManager.h"
#include "../../../component/ray_casting/IBoundingVolumeComponent.h"
#include "../../../component/terrain/ITerrainComponent.h"
#include "../../../component/transform/ITransformComponent.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::shadow {
TerrainShadowsRenderer::TerrainShadowsRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void TerrainShadowsRenderer::Init()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ *device }
        .AddShaderStagePaths({
            { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/shadow/terrain_shadows_vert.spv") }
        })
        .AddVertexInputAttributeDescriptions({
            prev::util::vk::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 }))
        })
        .AddVertexInputBindingDescriptions({
            prev::util::vk::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
        })
        .AddDescriptorSets({
            { "ubo", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT }
        })
	    .SetDescriptorPoolCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Terrain Shadows Shader created\n");

    m_pipeline = std::make_unique<pipeline::TerrainShadowsPipeline>(*device, *m_shader, *m_renderPass);
    m_pipeline->Init();

    LOGI("Terrain Shadows Pipeline created\n");

    m_uniformsPool = std::make_unique<prev::render::buffer::UniformBufferRing<Uniforms>>(*allocator);
    m_uniformsPool->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));
}

void TerrainShadowsRenderer::BeforeRender(const ShadowsRenderContext& renderContext)
{
}

void TerrainShadowsRenderer::PreRender(const ShadowsRenderContext& renderContext)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void TerrainShadowsRenderer::Render(const ShadowsRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (node->GetTags().HasAll({ TAG_TERRAIN_RENDER_COMPONENT, TAG_TRANSFORM_COMPONENT })) {
        bool visible{ true };
        if (prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
            visible = prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(renderContext.frustum);
        }

        if (visible) {
            const auto transformComponent = prev::scene::component::ComponentRepository<prev_test::component::transform::ITransformComponent>::Instance().Get(node->GetId());
            const auto terrainComponent = prev::scene::component::ComponentRepository<prev_test::component::terrain::ITerrainComponenet>::Instance().Get(node->GetId());
            auto ubo = m_uniformsPool->GetNext();

            Uniforms uniforms{};
            uniforms.projectionMatrix = renderContext.projectionMatrix;
            uniforms.viewMatrix = renderContext.viewMatrix;
            uniforms.modelMatrix = transformComponent->GetWorldTransformScaled();
            ubo->Update(&uniforms);

            m_shader->Bind("ubo", *ubo);

            const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
            const VkBuffer vertexBuffers[] = { *terrainComponent->GetModel()->GetVertexBuffer() };
            const VkDeviceSize offsets[] = { 0 };

            vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.commandBuffer, *terrainComponent->GetModel()->GetIndexBuffer(), 0, terrainComponent->GetModel()->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.commandBuffer, terrainComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
        }
    }
}

void TerrainShadowsRenderer::PostRender(const ShadowsRenderContext& renderContext)
{
}

void TerrainShadowsRenderer::AfterRender(const ShadowsRenderContext& renderContext)
{
}

void TerrainShadowsRenderer::ShutDown()
{
    m_pipeline->ShutDown();
    m_pipeline = nullptr;
    m_shader = nullptr;
}
} // namespace prev_test::render::renderer::shadow
