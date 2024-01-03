#include "AnimationShadowsRenderer.h"
#include "pipeline/AnimatedShadowsPipeline.h"
#include "shader/AnimatedShadowsShader.h"

#include "../../../component/ray_casting/IBoundingVolumeComponent.h"
#include "../../../component/render/IAnimationRenderComponent.h"
#include "../../../component/transform/ITransformComponent.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::shadow {
AnimationShadowsRenderer::AnimationShadowsRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void AnimationShadowsRenderer::Init()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev::render::shader::ShaderFactory shaderFactory;
    m_shader = shaderFactory.CreateShaderFromFiles<shader::AnimatedShadowsShader>(*device, shader::AnimatedShadowsShader::GetPaths());
    m_shader->Init();
    m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Animation Shadows Shader created\n");

    m_pipeline = std::make_unique<pipeline::AnimatedShadowsPipeline>(*device, *m_shader, *m_renderPass);
    m_pipeline->Init();

    LOGI("Animation Shadows Pipeline created\n");

    m_uniformsPool = std::make_unique<prev::render::buffer::UniformBufferRing<Uniforms>>(*allocator);
    m_uniformsPool->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));
}

void AnimationShadowsRenderer::BeforeRender(const ShadowsRenderContext& renderContext)
{
}

void AnimationShadowsRenderer::PreRender(const ShadowsRenderContext& renderContext)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void AnimationShadowsRenderer::Render(const ShadowsRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (node->GetTags().HasAny({ TAG_ANIMATION_RENDER_COMPONENT, TAG_ANIMATION_TEXTURELESS_RENDER_COMPONENT }) && node->GetTags().HasAll({ TAG_TRANSFORM_COMPONENT })) {
        bool visible{ true };
        if (prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
            visible = prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(renderContext.frustum);
        }

        const auto renderComponent = prev::scene::component::ComponentRepository<prev_test::component::render::IAnimationRenderComponent>::Instance().Get(node->GetId());
        if (renderComponent->CastsShadows() && visible) {
            RenderMeshNode(renderContext, node, renderComponent->GetModel()->GetMesh()->GetRootNode());
        }
    }
}

void AnimationShadowsRenderer::PostRender(const ShadowsRenderContext& renderContext)
{
}

void AnimationShadowsRenderer::AfterRender(const ShadowsRenderContext& renderContext)
{
}

void AnimationShadowsRenderer::ShutDown()
{
    m_pipeline->ShutDown();
    m_pipeline = nullptr;

    m_shader->ShutDown();
    m_shader = nullptr;
}

void AnimationShadowsRenderer::RenderMeshNode(const ShadowsRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const prev_test::render::MeshNode& meshNode)
{
    const auto transformComponent = prev::scene::component::ComponentRepository<prev_test::component::transform::ITransformComponent>::Instance().Get(node->GetId());
    const auto renderComponent = prev::scene::component::ComponentRepository<prev_test::component::render::IAnimationRenderComponent>::Instance().Get(node->GetId());

    const auto model = renderComponent->GetModel();
    const auto mesh = model->GetMesh();
    const auto animation = renderComponent->GetCurrentAnimation();

    const auto& meshParts{ mesh->GetMeshParts() };
    for (const auto meshPartIndex : meshNode.meshPartIndices) {
        const auto& meshPart = meshParts[meshPartIndex];
        const auto& animationPart = animation->GetAnimationPart(meshPartIndex);

        auto ubo = m_uniformsPool->GetNext();

        Uniforms uniforms{};
        const auto& bones = animationPart->GetBoneTransforms();
        for (size_t i = 0; i < bones.size(); i++) {
            uniforms.bones[i] = bones[i];
        }
        uniforms.projectionMatrix = renderContext.projectionMatrix;
        uniforms.viewMatrix = renderContext.viewMatrix;
        uniforms.modelMatrix = transformComponent->GetWorldTransformScaled() * meshNode.transform;
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
        RenderMeshNode(renderContext, node, childMeshNode);
    }
}
} // namespace prev_test::render::renderer::shadow
