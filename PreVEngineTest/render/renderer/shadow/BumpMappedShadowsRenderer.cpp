#include "BumpMappedShadowsRenderer.h"
#include "pipeline/BumpMappedShadowsPipeline.h"
#include "shader/BumpMappedShadowsShader.h"

#include "../../../component/ray_casting/IBoundingVolumeComponent.h"
#include "../../../component/render/IRenderComponent.h"
#include "../../../component/transform/ITransformComponent.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/core/memory/buffer/UniformBuffer.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::shadow {
BumpMappedShadowsRenderer::BumpMappedShadowsRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void BumpMappedShadowsRenderer::Init()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev::render::shader::ShaderFactory shaderFactory;
    m_shader = shaderFactory.CreateShaderFromFiles<shader::BumpMappedShadowsShader>(*device, shader::BumpMappedShadowsShader::GetPaths());
    m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Bump Mapped Shadows Shader created\n");

    m_pipeline = std::make_unique<pipeline::BumpMappedShadowsPipeline>(*device, *m_shader, *m_renderPass);
    m_pipeline->Init();

    LOGI("Bump Mapped Shadows Pipeline created\n");

    m_uniformsPool = std::make_unique<prev::core::memory::buffer::UBOPool<Uniforms> >(*allocator);
    m_uniformsPool->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));
}

void BumpMappedShadowsRenderer::PreRender(const prev::render::RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void BumpMappedShadowsRenderer::BeforeRender(const prev::render::RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext)
{
}

void BumpMappedShadowsRenderer::Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const ShadowsRenderContextUserData& shadowsRenderContext)
{
    if (node->GetTags().HasAll({ TAG_TRANSFORM_COMPONENT }) && node->GetTags().HasAny({ TAG_RENDER_NORMAL_MAPPED_COMPONENT, TAG_RENDER_PARALLAX_MAPPED_COMPONENT, TAG_RENDER_CONE_STEP_MAPPED_COMPONENT })) {
        bool visible{ true };
        if (prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
            visible = prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(shadowsRenderContext.frustum);
        }

        const auto renderComponent = prev::scene::component::ComponentRepository<prev_test::component::render::IRenderComponent>::Instance().Get(node->GetId());
        if (renderComponent->CastsShadows() && visible) {
            const auto nodeRenderComponent = prev::scene::component::ComponentRepository<prev_test::component::render::IRenderComponent>::Instance().Get(node->GetId());
            RenderMeshNode(renderContext, node, shadowsRenderContext, nodeRenderComponent->GetModel()->GetMesh()->GetRootNode());
        }
    }

    for (auto& child : node->GetChildren()) {
        Render(renderContext, child, shadowsRenderContext);
    }
}

void BumpMappedShadowsRenderer::PostRender(const prev::render::RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext)
{
}

void BumpMappedShadowsRenderer::AfterRender(const prev::render::RenderContext& renderContext, const ShadowsRenderContextUserData& renderContextUserData)
{
}

void BumpMappedShadowsRenderer::ShutDown()
{
    m_pipeline->ShutDown();

    m_shader->ShutDown();
}

void BumpMappedShadowsRenderer::RenderMeshNode(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const ShadowsRenderContextUserData& renderContextUserData, const prev_test::render::MeshNode& meshNode)
{
    const auto transformComponent = prev::scene::component::ComponentRepository<prev_test::component::transform::ITransformComponent>::Instance().Get(node->GetId());
    const auto renderComponent = prev::scene::component::ComponentRepository<prev_test::component::render::IRenderComponent>::Instance().Get(node->GetId());

    const auto model = renderComponent->GetModel();
    const auto mesh = model->GetMesh();

    const auto meshParts = mesh->GetMeshParts();
    for (const auto meshPartIndex : meshNode.meshPartIndices) {
        const auto meshPart = meshParts.at(meshPartIndex);

        auto ubo = m_uniformsPool->GetNext();

        Uniforms uniforms{};
        uniforms.projectionMatrix = renderContextUserData.projectionMatrix;
        uniforms.viewMatrix = renderContextUserData.viewMatrix;
        uniforms.modelMatrix = transformComponent->GetWorldTransformScaled();
        ubo->Update(&uniforms);

        m_shader->Bind("ubo", *ubo);

        const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
        const VkBuffer vertexBuffers[] = { *model->GetVertexBuffer() };
        const VkDeviceSize offsets[] = { meshPart.firstVertexIndex * mesh->GetVertexLayout().GetStride() };

        vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(renderContext.commandBuffer, *model->GetIndexBuffer(), 0, model->GetIndexBuffer()->GetIndexType());
        vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(renderContext.commandBuffer, meshPart.indicesCount, 1, meshPart.firstIndicesIndex, 0, 0);
    }

    for (const auto& childMeshNode : meshNode.children) {
        RenderMeshNode(renderContext, node, renderContextUserData, childMeshNode);
    }
}
} // namespace prev_test::render::renderer::shadow
