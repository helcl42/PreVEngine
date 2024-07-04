#include "RayCastDebugRenderer.h"

#ifdef RENDER_RAYCASTS

#include "pipeline/RayCastDebugPipeline.h"

#include "../../../common/AssetManager.h"
#include "../../../component/ray_casting/IRayCasterComponent.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::debug {
RayCastDebugRenderer::RayCastDebugRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void RayCastDebugRenderer::Init()
{
    auto device = prev::core::DeviceProvider::Instance().GetDevice();
    auto allocator = prev::core::AllocatorProvider::Instance().GetAllocator();

    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ *device }
        .AddShaderStagePaths({
            { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/debug/raycast_debug_vert.spv") },
            { VK_SHADER_STAGE_GEOMETRY_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/debug/raycast_debug_geom.spv") },
            { VK_SHADER_STAGE_FRAGMENT_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/debug/raycast_debug_frag.spv") }
        })
        .AddVertexInputAttributeDescriptions({
            prev::util::vk::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0)
        })
        .AddVertexInputBindingDescriptions({
            prev::util::vk::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
        })
        .AddDescriptorSets({
            { "uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT },
            { "uboGS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_GEOMETRY_BIT },
            { "uboFS", 2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT }
        })
	    .SetDescriptorPoolCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("RayCast Debug Shader created\n");

    m_pipeline = std::make_unique<pipeline::RayCastDebugPipeline>(*device, *m_shader, *m_renderPass);
    m_pipeline->Init();

    LOGI("RayCast Debug Pipeline created\n");

    m_uniformsPoolVS = std::make_unique<prev::render::buffer::UniformBufferRing<UniformsVS>>(*allocator);
    m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));

    m_uniformsPoolGS = std::make_unique<prev::render::buffer::UniformBufferRing<UniformsGS>>(*allocator);
    m_uniformsPoolGS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));

    m_uniformsPoolFS = std::make_unique<prev::render::buffer::UniformBufferRing<UniformsFS>>(*allocator);
    m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));
}

void RayCastDebugRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
}

void RayCastDebugRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void RayCastDebugRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (node->GetTags().HasAll({ TAG_RAYCASTER_COMPONENT })) {
        const auto rayCastingComponent = prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IRayCasterComponent>::Instance().Get(node->GetId());

        auto uboVS = m_uniformsPoolVS->GetNext();

        UniformsVS uniformsVS{};
        uniformsVS.color = glm::vec3(1.0, 0.0, 0.0);

        uboVS->Update(&uniformsVS);

        auto uboGS = m_uniformsPoolGS->GetNext();

        UniformsGS uniformsGS{};
        uniformsGS.projectionMatrix = renderContext.projectionMatrix;
        uniformsGS.viewMatrix = renderContext.viewMatrix;
        uniformsGS.modelMatrix = glm::mat4(1.0f);

        uboGS->Update(&uniformsGS);

        auto uboFS = m_uniformsPoolFS->GetNext();

        UniformsFS uniformsFS{};
        uniformsFS.alpha = 0.7f;

        uboFS->Update(&uniformsFS);

        m_shader->Bind("uboVS", *uboVS);
        m_shader->Bind("uboGS", *uboGS);
        m_shader->Bind("uboFS", *uboFS);

        const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
        const VkBuffer vertexBuffers[] = { *rayCastingComponent->GetModel()->GetVertexBuffer() };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(renderContext.commandBuffer, *rayCastingComponent->GetModel()->GetIndexBuffer(), 0, rayCastingComponent->GetModel()->GetIndexBuffer()->GetIndexType());
        vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(renderContext.commandBuffer, rayCastingComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
    }
}

void RayCastDebugRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void RayCastDebugRenderer::AfterRender(const NormalRenderContext& renderContext)
{
}

void RayCastDebugRenderer::ShutDown()
{
    m_pipeline->ShutDown();
    m_pipeline = nullptr;

    m_shader->ShutDown();
    m_shader = nullptr;
}
} // namespace prev_test::render::renderer::debug

#endif