#include "TerrainBumplMappedShadowsRenderer.h"

#include "pipeline/TerrainBumpMappedShadowsPipeline.h"
#include "shader/TerrainBumpMappedShadowsShader.h"

#include "../../../common/AssetManager.h"
#include "../../../component/terrain/ITerrainComponent.h"
#include "../../../component/transform/ITransformComponent.h"

#include <prev/core/DeviceProvider.h>
#include <prev/core/memory/buffer/UniformBuffer.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/AllocatorProvider.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>

#include <memory>

namespace prev_test::render::renderer::shadow {
TerrainBumplMappedShadowsRenderer::TerrainBumplMappedShadowsRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void TerrainBumplMappedShadowsRenderer::Init()
{
    auto device = prev::core::DeviceProvider::Instance().GetDevice();
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    prev::render::shader::ShaderFactory shaderFactory;
    m_shader = shaderFactory.CreateShaderFromFiles<prev_test::render::renderer::shadow::shader::TerrainBumpMappedShadowsShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/terrain_bump_mapped_shadows_vert.spv") } });
    m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Terrain Bump Mapped Shadows Shader created\n");

    m_pipeline = std::make_unique<prev_test::render::renderer::shadow::pipeline::TerrainBumpMappedShadowsPipeline>(*device, *m_renderPass, *m_shader);
    m_pipeline->Init();

    LOGI("Terrain Bump Mapped Shadows Pipeline created\n");

    m_uniformsPool = std::make_unique<prev::core::memory::buffer::UBOPool<Uniforms> >(*allocator);
    m_uniformsPool->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
}

void TerrainBumplMappedShadowsRenderer::BeforeRender(const prev::render::RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext)
{
}

void TerrainBumplMappedShadowsRenderer::PreRender(const prev::render::RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext)
{
    const VkRect2D scissor{ { 0, 0 }, shadowsRenderContext.extent };
    const VkViewport viewport{ 0, 0, static_cast<float>(shadowsRenderContext.extent.width), static_cast<float>(shadowsRenderContext.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void TerrainBumplMappedShadowsRenderer::Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode<SceneNodeFlags> >& node, const ShadowsRenderContextUserData& shadowsRenderContext)
{
    if (node->GetFlags().HasAll(prev::common::FlagSet<SceneNodeFlags>{ SceneNodeFlags::TRANSFORM_COMPONENT }) && node->GetFlags().HasAny(prev::common::FlagSet<SceneNodeFlags>{ SceneNodeFlags::TERRAIN_NORMAL_MAPPED_RENDER_COMPONENT | SceneNodeFlags::TERRAIN_PARALLAX_MAPPED_RENDER_COMPONENT | SceneNodeFlags::TERRAIN_CONE_STEP_MAPPED_RENDER_COMPONENT })) {
        bool visible = true;
        if (prev::scene::component::ComponentRepository<IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
            visible = prev::scene::component::ComponentRepository<IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(shadowsRenderContext.frustum);
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

    for (auto child : node->GetChildren()) {
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

    m_shader->ShutDown();
}
} // namespace prev_test::render::renderer::shadow
