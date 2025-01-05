#include "AnimationBumpMappedShadowsRenderer.h"

#include "../RendererUtils.h"

#include "../../../common/AssetManager.h"
#include "../../../component/render/IAnimationRenderComponent.h"
#include "../../../component/transform/ITransformComponent.h"

#include <prev/render/pipeline/PipelineBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::shadow {
AnimationBumpMappedShadowsRenderer::AnimationBumpMappedShadowsRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass)
    : m_device{ device }
    , m_allocator{ allocator }
    , m_renderPass{ renderPass }
{
}

void AnimationBumpMappedShadowsRenderer::Init()
{
    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/shadow/animation_bump_mapped_shadows_vert.spv") }
        })
        .AddVertexInputAttributeDescriptions({
            prev::util::vk::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32B32A32_SINT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 })),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 4, VK_FORMAT_R32G32B32A32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4 })),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 5, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4 })),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 6, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC3 }))
        })
        .AddVertexInputBindingDescriptions({
            prev::util::vk::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
        })
        .AddDescriptorSets({
            { "ubo", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT }
        })
	    .SetDescriptorPoolCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Animation Bump Mapped Shadows Shader created");

    // clang-format off
    m_pipeline = prev::render::pipeline::GraphicsPipelineBuilder{ m_device, *m_shader, m_renderPass }
        .SetPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetDepthTestEnabled(true)
        .SetDepthWriteEnabled(true)
        .SetBlendingModeEnabled(false)
        .SetAdditiveBlendingEnabled(false)
        .SetPolygonMode(VK_POLYGON_MODE_FILL)
        .SetCullingMode(VK_CULL_MODE_BACK_BIT)
        .Build();
    // clang-format on

    LOGI("Animation Bump Mapped Shadows Pipeline created");

    m_uniformsPool = std::make_unique<prev::render::buffer::UniformBufferRing<Uniforms>>(m_allocator);
    m_uniformsPool->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(m_device.GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
}

void AnimationBumpMappedShadowsRenderer::BeforeRender(const ShadowsRenderContext& renderContext)
{
}

void AnimationBumpMappedShadowsRenderer::PreRender(const ShadowsRenderContext& renderContext)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void AnimationBumpMappedShadowsRenderer::Render(const ShadowsRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (node->GetTags().HasAll({ TAG_TRANSFORM_COMPONENT }) && node->GetTags().HasAny({ TAG_ANIMATION_NORMAL_MAPPED_RENDER_COMPONENT, TAG_ANIMATION_CONE_STEP_MAPPED_RENDER_COMPONENT })) {
        const auto renderComponent = prev::scene::component::ComponentRepository<prev_test::component::render::IAnimationRenderComponent>::Instance().Get(node->GetId());
        if (renderComponent->CastsShadows() && prev_test::render::renderer::IsVisible(&renderContext.frustum, 1, node->GetId())) {
            RenderMeshNode(renderContext, node, renderComponent->GetModel()->GetMesh()->GetRootNode());
        }
    }
}

void AnimationBumpMappedShadowsRenderer::PostRender(const ShadowsRenderContext& renderContext)
{
}

void AnimationBumpMappedShadowsRenderer::AfterRender(const ShadowsRenderContext& renderContext)
{
}

void AnimationBumpMappedShadowsRenderer::ShutDown()
{
    m_pipeline = nullptr;
    m_shader = nullptr;
}

void AnimationBumpMappedShadowsRenderer::RenderMeshNode(const ShadowsRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const prev_test::render::MeshNode& meshNode)
{
    const auto transformComponent = prev::scene::component::ComponentRepository<prev_test::component::transform::ITransformComponent>::Instance().Get(node->GetId());
    const auto renderComponent = prev::scene::component::ComponentRepository<prev_test::component::render::IAnimationRenderComponent>::Instance().Get(node->GetId());

    const auto model = renderComponent->GetModel();
    const auto mesh = model->GetMesh();
    const auto animation = renderComponent->GetCurrentAnimation();

    const auto& meshParts{ mesh->GetMeshParts() };
    for (const auto meshPartIndex : meshNode.meshPartIndices) {
        const auto& meshPart = meshParts[meshPartIndex];
        const auto& animationClip = animation->GetClip(meshPartIndex);

        auto ubo = m_uniformsPool->GetNext();

        Uniforms uniforms{};
        const auto& bones = animationClip.GetBoneTransforms();
        for (size_t i = 0; i < bones.size(); ++i) {
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
