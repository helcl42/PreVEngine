#include "AnimationShadowsRenderer.h"

#include "pipeline/AnimatedShadowsPipeline.h"
#include "shader/AnimatedShadowsShader.h"

#include <prev/core/DeviceProvider.h>
#include <prev/core/memory/buffer/UniformBuffer.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/AllocatorProvider.h>

#include <memory>

namespace prev_test::render::shadow {
AnimationShadowsRenderer::AnimationShadowsRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void AnimationShadowsRenderer::Init()
{
    auto device = prev::core::DeviceProvider::Instance().GetDevice();
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    prev::render::shader::ShaderFactory shaderFactory;
    m_shader = shaderFactory.CreateShaderFromFiles<prev_test::render::shadow::shader::AnimatedShadowsShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, AssetManager::Instance().GetAssetPath("Shaders/animation_shadows_vert.spv") } });
    m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Animation Shadows Shader created\n");

    m_pipeline = std::make_unique<prev_test::render::shadow::pipeline::AnimatedShadowsPipeline>(*device, *m_renderPass, *m_shader);
    m_pipeline->Init();

    LOGI("Animation Shadows Pipeline created\n");

    m_uniformsPool = std::make_unique<prev::core::memory::buffer::UBOPool<Uniforms> >(*allocator);
    m_uniformsPool->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
}

void AnimationShadowsRenderer::BeforeRender(const prev::render::RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext)
{
}

void AnimationShadowsRenderer::PreRender(const prev::render::RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext)
{
    const VkRect2D scissor{ { 0, 0 }, shadowsRenderContext.extent };
    const VkViewport viewport{ 0, 0, static_cast<float>(shadowsRenderContext.extent.width), static_cast<float>(shadowsRenderContext.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void AnimationShadowsRenderer::Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode<SceneNodeFlags> >& node, const ShadowsRenderContextUserData& shadowsRenderContext)
{
    if (node->GetFlags().HasAll(prev::common::FlagSet<SceneNodeFlags>{ SceneNodeFlags::ANIMATION_RENDER_COMPONENT | SceneNodeFlags::TRANSFORM_COMPONENT })) {
        bool visible = true;
        if (prev::scene::component::ComponentRepository<IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
            visible = prev::scene::component::ComponentRepository<IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(shadowsRenderContext.frustum);
        }

        const auto transformComponent = prev::scene::component::ComponentRepository<ITransformComponent>::Instance().Get(node->GetId());
        const auto renderComponent = prev::scene::component::ComponentRepository<IAnimationRenderComponent>::Instance().Get(node->GetId());
        if (renderComponent->CastsShadows() && visible) {
            const auto vertexStride = renderComponent->GetModel()->GetMesh()->GetVertexLayout().GetStride();
            const auto meshParts = renderComponent->GetModel()->GetMesh()->GetMeshParts();
            for (const auto& meshPart : meshParts) {
                const auto modelMatrix = transformComponent->GetWorldTransformScaled() * meshPart.transform;

                auto ubo = m_uniformsPool->GetNext();

                Uniforms uniforms{};
                const auto& bones = renderComponent->GetAnimation()->GetBoneTransforms();
                for (size_t i = 0; i < bones.size(); i++) {
                    uniforms.bones[i] = bones[i];
                }
                uniforms.projectionMatrix = shadowsRenderContext.projectionMatrix;
                uniforms.viewMatrix = shadowsRenderContext.viewMatrix;
                uniforms.modelMatrix = modelMatrix;
                ubo->Update(&uniforms);

                m_shader->Bind("ubo", *ubo);

                const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
                const VkBuffer vertexBuffers[] = { *renderComponent->GetModel()->GetVertexBuffer() };
                const VkDeviceSize offsets[] = { meshPart.firstVertexIndex * vertexStride };

                vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.commandBuffer, *renderComponent->GetModel()->GetIndexBuffer(), 0, renderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.commandBuffer, meshPart.indicesCount, 1, meshPart.firstIndicesIndex, 0, 0);
            }
        }
    }

    for (auto child : node->GetChildren()) {
        Render(renderContext, child, shadowsRenderContext);
    }
}

void AnimationShadowsRenderer::PostRender(const prev::render::RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext)
{
}

void AnimationShadowsRenderer::AfterRender(const prev::render::RenderContext& renderContext, const ShadowsRenderContextUserData& renderContextUserData)
{
}

void AnimationShadowsRenderer::ShutDown()
{
    m_pipeline->ShutDown();

    m_shader->ShutDown();
}
} // namespace prev_test::render::shadow