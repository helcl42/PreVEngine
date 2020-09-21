#include "SkyBoxRenderer.h"

#include "pipeline/SkyBoxPipeline.h"
#include "shader/SkyBoxShader.h"

#include "../../Sky.h"

#include <prev/core/DeviceProvider.h>
#include <prev/core/memory/buffer/UniformBuffer.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/AllocatorProvider.h>

#include <memory>

namespace prev_test::render::sky {
SkyBoxRenderer::SkyBoxRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void SkyBoxRenderer::Init()
{
    auto device = prev::core::DeviceProvider::Instance().GetDevice();
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    prev::render::shader::ShaderFactory shaderFactory;
    m_shader = shaderFactory.CreateShaderFromFiles<prev_test::render::sky::shader::SkyBoxShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, AssetManager::Instance().GetAssetPath("Shaders/skybox_vert.spv") }, { VK_SHADER_STAGE_FRAGMENT_BIT, AssetManager::Instance().GetAssetPath("Shaders/skybox_frag.spv") } });
    m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Skybox Shader created\n");

    m_pipeline = std::make_unique<prev_test::render::sky::pipeline::SkyBoxPipeline>(*device, *m_renderPass, *m_shader);
    m_pipeline->Init();

    LOGI("Skybox Pipeline created\n");

    m_uniformsPoolVS = std::make_unique<prev::core::memory::buffer::UBOPool<UniformsVS> >(*allocator);
    m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

    m_uniformsPoolFS = std::make_unique<prev::core::memory::buffer::UBOPool<UniformsFS> >(*allocator);
    m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
}

void SkyBoxRenderer::BeforeRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void SkyBoxRenderer::PreRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
    const VkRect2D scissor{ { 0, 0 }, renderContext.fullExtent };
    const VkViewport viewport{ 0, 0, static_cast<float>(renderContextUserData.extent.width), static_cast<float>(renderContextUserData.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void SkyBoxRenderer::Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData)
{
    if (node->GetFlags().HasAll(prev::common::FlagSet<SceneNodeFlags>{ SceneNodeFlags::SKYBOX_RENDER_COMPONENT | SceneNodeFlags::TRANSFORM_COMPONENT })) {
        const auto transformComponent = prev::scene::component::ComponentRepository<ITransformComponent>::Instance().Get(node->GetId());
        const auto skyBoxComponent = prev::scene::component::ComponentRepository<ISkyBoxComponent>::Instance().Get(node->GetId());

        auto uboVS = m_uniformsPoolVS->GetNext();

        UniformsVS uniformsVS{};
        uniformsVS.projectionMatrix = renderContextUserData.projectionMatrix;
        uniformsVS.viewMatrix = renderContextUserData.viewMatrix;
        uniformsVS.modelMatrix = transformComponent->GetWorldTransformScaled();

        // TODO should we use CLIP_PLANE here?

        uboVS->Update(&uniformsVS);

        auto uboFS = m_uniformsPoolFS->GetNext();

        UniformsFS uniformsFS{};
        uniformsFS.fogColor = FOG_COLOR;
        uniformsFS.lowerLimit = glm::vec4(0.0f);
        uniformsFS.upperLimit = glm::vec4(0.03f);
        uboFS->Update(&uniformsFS);

        m_shader->Bind("cubeMap1", *skyBoxComponent->GetMaterial()->GetImageBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        m_shader->Bind("uboVS", *uboVS);
        m_shader->Bind("uboFS", *uboFS);

        const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
        const VkBuffer vertexBuffers[] = { *skyBoxComponent->GetModel()->GetVertexBuffer() };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(renderContext.commandBuffer, *skyBoxComponent->GetModel()->GetIndexBuffer(), 0, skyBoxComponent->GetModel()->GetIndexBuffer()->GetIndexType());
        vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(renderContext.commandBuffer, skyBoxComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
    }

    for (auto child : node->GetChildren()) {
        Render(renderContext, child, renderContextUserData);
    }
}

void SkyBoxRenderer::PostRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void SkyBoxRenderer::AfterRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void SkyBoxRenderer::ShutDown()
{
    m_shader->ShutDown();

    m_pipeline->ShutDown();
}
} // namespace prev_test::render::sky