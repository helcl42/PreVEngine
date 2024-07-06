#include "ParticlesRenderer.h"

#include "../../../common/AssetManager.h"
#include "../../../component/particle/IParticleSystemComponent.h"
#include "../../../component/ray_casting/IBoundingVolumeComponent.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/render/pipeline/PipelineBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::particle {
ParticlesRenderer::ParticlesRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void ParticlesRenderer::Init()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ *device }
        .AddShaderStagePaths({
            { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/particle/particles_vert.spv") },
            { VK_SHADER_STAGE_FRAGMENT_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/particle/particles_frag.spv") }
        })
        .AddVertexInputAttributeDescriptions({
            prev::util::vk::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),

            prev::util::vk::CreateVertexInputAttributeDescription(1, 3, VK_FORMAT_R32G32B32_SFLOAT, 0),
            prev::util::vk::CreateVertexInputAttributeDescription(1, 4, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            prev::util::vk::CreateVertexInputAttributeDescription(1, 5, VK_FORMAT_R32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),
            prev::util::vk::CreateVertexInputAttributeDescription(1, 6, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::FLOAT })),
            prev::util::vk::CreateVertexInputAttributeDescription(1, 7, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::FLOAT, VertexLayoutComponent::VEC2 })),
            prev::util::vk::CreateVertexInputAttributeDescription(1, 8, VK_FORMAT_R32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::FLOAT, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC2 }))
        })
        .AddVertexInputBindingDescriptions({
            prev::util::vk::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX),
            prev::util::vk::CreateVertexInputBindingDescription(1, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::FLOAT, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC2, VertexLayoutComponent::FLOAT }), VK_VERTEX_INPUT_RATE_INSTANCE)
        })
        .AddDescriptorSets({
            { "uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT },
            { "uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT },
            { "colorSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT }
        })
	    .SetDescriptorPoolCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Particles Shader created\n");

    // clang-format off
    m_pipeline = prev::render::pipeline::GraphicsPipelineBuilder{ *device, *m_shader, *m_renderPass }
        .SetPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetDepthTestEnabled(true)
        .SetDepthWriteEnabled(false)
        .SetBlendingModeEnabled(true)
        .SetAdditiveBlendingEnabled(true)
        .SetPolygonMode(VK_POLYGON_MODE_FILL)
        .Build();
    // clang-format on

    LOGI("Particles Pipeline created\n");

    m_uniformsPoolVS = std::make_unique<prev::render::buffer::UniformBufferRing<UniformsVS>>(*allocator);
    m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));

    m_uniformsPoolFS = std::make_unique<prev::render::buffer::UniformBufferRing<UniformsFS>>(*allocator);
    m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));
}

void ParticlesRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
}

void ParticlesRenderer::PreRender(const NormalRenderContext& renderContext)
{
}

void ParticlesRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (node->GetTags().HasAll({ TAG_PARTICLE_SYSTEM_COMPONENT })) {
        bool visible{ true };
        if (prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
            visible = prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(renderContext.frustum);
        }

        if (visible) {
            const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
            const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

            vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
            vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
            vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);

            const auto particlesComponent = prev::scene::component::ComponentRepository<prev_test::component::particle::IParticleSystemComponent>::Instance().Get(node->GetId());

            auto uboVS = m_uniformsPoolVS->GetNext();
            UniformsVS uniformsVS{};
            uniformsVS.projectionMatrix = renderContext.projectionMatrix;
            uniformsVS.viewMatrix = renderContext.viewMatrix;
            uniformsVS.textureNumberOfRows = particlesComponent->GetMaterial()->GetAtlasNumberOfRows();
            uboVS->Update(&uniformsVS);

            auto uboFS = m_uniformsPoolFS->GetNext();
            UniformsFS uniformsFS{};
            uniformsFS.color = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
            uboFS->Update(&uniformsFS);

            m_shader->Bind("uboVS", *uboVS);
            m_shader->Bind("uboFS", *uboFS);
            m_shader->Bind("colorSampler", *particlesComponent->GetMaterial()->GetImageBuffer(), *particlesComponent->GetMaterial()->GetSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
            const VkBuffer vertexBuffers[] = { *particlesComponent->GetModel()->GetVertexBuffer() };
            const VkBuffer instanceBuffers[] = { *particlesComponent->GetVertexBuffer() };
            const VkDeviceSize offsets[] = { 0 };

            vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindVertexBuffers(renderContext.commandBuffer, 1, 1, instanceBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.commandBuffer, *particlesComponent->GetModel()->GetIndexBuffer(), 0, particlesComponent->GetModel()->GetIndexBuffer()->GetIndexType());

            vkCmdDrawIndexed(renderContext.commandBuffer, particlesComponent->GetModel()->GetIndexBuffer()->GetCount(), static_cast<uint32_t>(particlesComponent->GetParticles().size()), 0, 0, 0);
        }
    }
}

void ParticlesRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void ParticlesRenderer::AfterRender(const NormalRenderContext& renderContext)
{
}

void ParticlesRenderer::ShutDown()
{
    m_pipeline = nullptr;
    m_shader = nullptr;
}
} // namespace prev_test::render::renderer::particle
