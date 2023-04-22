#include "TerrainBumplMappedShadowsRenderer.h"
#include "pipeline/TerrainBumpMappedShadowsPipeline.h"
#include "shader/TerrainBumpMappedShadowsShader.h"

#include "../../../component/ray_casting/IBoundingVolumeComponent.h"
#include "../../../component/terrain/ITerrainComponent.h"
#include "../../../component/transform/ITransformComponent.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/core/memory/buffer/UniformBuffer.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::shadow {
TerrainBumplMappedShadowsRenderer::TerrainBumplMappedShadowsRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void TerrainBumplMappedShadowsRenderer::Init()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev::render::shader::ShaderFactory shaderFactory;
    m_shader = shaderFactory.CreateShaderFromFiles<shader::TerrainBumpMappedShadowsShader>(*device, shader::TerrainBumpMappedShadowsShader::GetPaths());
    m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Terrain Bump Mapped Shadows Shader created\n");

    m_pipeline = std::make_unique<pipeline::TerrainBumpMappedShadowsPipeline>(*device, *m_shader, *m_renderPass);
    m_pipeline->Init();

    LOGI("Terrain Bump Mapped Shadows Pipeline created\n");

    m_uniformsPool = std::make_unique<prev::core::memory::buffer::UBOPool<Uniforms>>(*allocator);
    m_uniformsPool->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));
}

void TerrainBumplMappedShadowsRenderer::BeforeRender(const prev::render::RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext)
{
}

void TerrainBumplMappedShadowsRenderer::PreRender(const prev::render::RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void TerrainBumplMappedShadowsRenderer::Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const ShadowsRenderContextUserData& shadowsRenderContext)
{
    if (node->GetTags().HasAll({ TAG_TRANSFORM_COMPONENT }) && node->GetTags().HasAny({ TAG_TERRAIN_NORMAL_MAPPED_RENDER_COMPONENT, TAG_TERRAIN_PARALLAX_MAPPED_RENDER_COMPONENT, TAG_TERRAIN_CONE_STEP_MAPPED_RENDER_COMPONENT })) {
        bool visible{ true };
        if (prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
            visible = prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(shadowsRenderContext.frustum);
        }

        if (visible) {
            const auto transformComponent = prev::scene::component::ComponentRepository<prev_test::component::transform::ITransformComponent>::Instance().Get(node->GetId());
            const auto terrainComponent = prev::scene::component::ComponentRepository<prev_test::component::terrain::ITerrainComponenet>::Instance().Get(node->GetId());
            auto ubo = m_uniformsPool->GetNext();

            Uniforms uniforms{};
            uniforms.projectionMatrix = shadowsRenderContext.projectionMatrix;
            uniforms.viewMatrix = shadowsRenderContext.viewMatrix;
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

    for (auto& child : node->GetChildren()) {
        Render(renderContext, child, shadowsRenderContext);
    }
}

void TerrainBumplMappedShadowsRenderer::PostRender(const prev::render::RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext)
{
}

void TerrainBumplMappedShadowsRenderer::AfterRender(const prev::render::RenderContext& renderContext, const ShadowsRenderContextUserData& renderContextUserData)
{
}

void TerrainBumplMappedShadowsRenderer::ShutDown()
{
    m_pipeline->ShutDown();
    m_pipeline = nullptr;

    m_shader->ShutDown();
    m_shader = nullptr;
}
} // namespace prev_test::render::renderer::shadow
