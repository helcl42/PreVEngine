#include "ParticlesRenderer.h"
#include "pipeline/ParticlesPipeline.h"
#include "shader/ParticlesShader.h"

#include "../../../component/particle/IParticleSystemComponent.h"
#include "../../VertexDataBuffer.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/core/memory/buffer/VertexBuffer.h>
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

    m_uniformsPoolVS = std::make_unique<prev::core::memory::buffer::UBOPool<UniformsVS>>(*allocator);
    m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));

    m_uniformsPoolFS = std::make_unique<prev::core::memory::buffer::UBOPool<UniformsFS>>(*allocator);
    m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));

    m_instanceDataBuffer = std::make_unique<prev::core::memory::buffer::VertexBuffer>(*allocator);
}

void ParticlesRenderer::BeforeRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void ParticlesRenderer::PreRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void ParticlesRenderer::Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const NormalRenderContextUserData& renderContextUserData)
{
    if (node->GetTags().HasAll({ TAG_PARTICLE_SYSTEM_COMPONENT })) {
        const auto particlesComponent = prev::scene::component::ComponentRepository<prev_test::component::particle::IParticleSystemComponent>::Instance().Get(node->GetId());
        const auto& particles = particlesComponent->GetParticles();

        if (particles.size() > 0) {
            const size_t singleInstanceSizeInBytes = sizeof(glm::mat4) + sizeof(glm::vec2) + sizeof(glm::vec2) + sizeof(float);
            prev_test::render::VertexDataBuffer instanceDataBuffer(singleInstanceSizeInBytes * particles.size());
            for (const auto& particle : particles) {
                instanceDataBuffer.Add(prev::util::math::CreateTransformationMatrix(particle->GetPosition(), glm::inverse(glm::quat_cast(renderContextUserData.viewMatrix)) * glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, particle->GetRotation()))), particle->GetScale()));
                instanceDataBuffer.Add(particle->GetCurrentStageTextureOffset());
                instanceDataBuffer.Add(particle->GetNextStageTextureOffset());
                instanceDataBuffer.Add(particle->GetStagesBlendFactor());
            }
            m_instanceDataBuffer->Data(instanceDataBuffer.GetData(), static_cast<uint32_t>(particles.size()), singleInstanceSizeInBytes);

            auto uboVS = m_uniformsPoolVS->GetNext();
            UniformsVS uniformsVS{};
            uniformsVS.projectionMatrix = renderContextUserData.projectionMatrix;
            uniformsVS.viewMatrix = renderContextUserData.viewMatrix;
            uniformsVS.textureNumberOfRows = particlesComponent->GetMaterial()->GetAtlasNumberOfRows();
            uboVS->Update(&uniformsVS);

            auto uboFS = m_uniformsPoolFS->GetNext();
            UniformsFS uniformsFS{};
            uniformsFS.color = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
            uboFS->Update(&uniformsFS);

            m_shader->Bind("uboVS", *uboVS);
            m_shader->Bind("uboFS", *uboFS);
            m_shader->Bind("colorSampler", particlesComponent->GetMaterial()->GetImageBuffer()->GetImageView(), *particlesComponent->GetMaterial()->GetImageSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
            const VkBuffer vertexBuffers[] = { *particlesComponent->GetModel()->GetVertexBuffer() };
            const VkBuffer instanceBuffers[] = { *m_instanceDataBuffer };
            const VkDeviceSize offsets[] = { 0 };

            vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindVertexBuffers(renderContext.commandBuffer, 1, 1, instanceBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.commandBuffer, *particlesComponent->GetModel()->GetIndexBuffer(), 0, particlesComponent->GetModel()->GetIndexBuffer()->GetIndexType());
            // vkCmdDrawIndexed(renderContext.commandBuffer, particlesComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
            vkCmdDrawIndexed(renderContext.commandBuffer, particlesComponent->GetModel()->GetIndexBuffer()->GetCount(), static_cast<uint32_t>(particles.size()), 0, 0, 0);
        }
    }

    for (auto& child : node->GetChildren()) {
        Render(renderContext, child, renderContextUserData);
    }
}

void ParticlesRenderer::PostRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void ParticlesRenderer::AfterRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
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
