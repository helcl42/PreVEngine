#include "ParticlesRenderer.h"
#include "pipeline/ParticlesPipeline.h"
#include "shader/ParticlesShader.h"

#include "../../../component/particle/IParticleSystemComponent.h"
#include "../../../component/ray_casting/IBoundingVolumeComponent.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::particle {
ParticlesRenderer::ParticlesRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void ParticlesRenderer::Init()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev::render::shader::ShaderFactory shaderFactory;
    m_shader = shaderFactory.CreateShaderFromFiles<shader::ParticlesShader>(*device, shader::ParticlesShader::GetPaths());
    m_shader->Init();
    m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Particles Shader created\n");

    m_pipeline = std::make_unique<pipeline::ParticlesPipeline>(*device, *m_shader, *m_renderPass);
    m_pipeline->Init();

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
            m_shader->Bind("colorSampler", particlesComponent->GetMaterial()->GetImageBuffer()->GetImageView(), *particlesComponent->GetMaterial()->GetSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

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
    m_pipeline->ShutDown();
    m_pipeline = nullptr;

    m_shader->ShutDown();
    m_shader = nullptr;
}
} // namespace prev_test::render::renderer::particle
