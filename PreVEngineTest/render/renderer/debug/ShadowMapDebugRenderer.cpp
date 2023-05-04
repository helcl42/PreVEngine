#include "ShadowMapDebugRenderer.h"
#include "pipeline/ShadowMapDebugPipeline.h"
#include "shader/ShadowMapDebugShader.h"

#include "../../../component/shadow/IShadowsComponent.h"
#include "../../../component/shadow/ShadowsCommon.h"
#include "../../mesh/MeshFactory.h"
#include "../../model/ModelFactory.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/core/memory/buffer/UniformBuffer.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::debug {
ShadowMapDebugRenderer::ShadowMapDebugRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void ShadowMapDebugRenderer::Init()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev::render::shader::ShaderFactory shaderFactory;
    m_shader = shaderFactory.CreateShaderFromFiles<shader::ShadowMapDebugShader>(*device, shader::ShadowMapDebugShader::GetPaths());
    m_shader->Init();
    m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("ShadowMapDebug Shader created\n");

    m_pipeline = std::make_unique<pipeline::ShadowMapDebugPipeline>(*device, *m_shader, *m_renderPass);
    m_pipeline->Init();

    LOGI("ShadowMapDebug Pipeline created\n");

    // create quad model
    prev_test::render::mesh::MeshFactory meshFactory{};
    auto quadMesh{ meshFactory.CreateQuad() };

    prev_test::render::model::ModelFactory modelFactory{};
    m_quadModel = modelFactory.Create(std::move(quadMesh), *allocator);
}

void ShadowMapDebugRenderer::BeforeRender(const prev::render::RenderContext& renderContext, const prev::render::DefaultRenderContextUserData& renderContextUserData)
{
}

void ShadowMapDebugRenderer::PreRender(const prev::render::RenderContext& renderContext, const prev::render::DefaultRenderContextUserData& renderContextUserData)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

// make a node with quad model & shadowMap texture ???
void ShadowMapDebugRenderer::Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const prev::render::DefaultRenderContextUserData& renderContextUserData)
{
    const auto shadows = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::shadow::IShadowsComponent>({ TAG_SHADOW });

    const auto& cascade{ shadows->GetCascade(static_cast<uint32_t>(m_cascadeIndex)) };
    PushConstantBlock pushConstBlock{ static_cast<uint32_t>(m_cascadeIndex), -cascade.startSplitDepth, -cascade.endSplitDepth };
    vkCmdPushConstants(renderContext.commandBuffer, m_pipeline->GetLayout(), VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantBlock), &pushConstBlock);

    m_shader->Bind("depthSampler", shadows->GetImageBuffer()->GetImageView(), *shadows->GetSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);

    const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
    const VkBuffer vertexBuffers[] = { *m_quadModel->GetVertexBuffer() };
    const VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(renderContext.commandBuffer, *m_quadModel->GetIndexBuffer(), 0, m_quadModel->GetIndexBuffer()->GetIndexType());
    vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

    vkCmdDrawIndexed(renderContext.commandBuffer, m_quadModel->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
}

void ShadowMapDebugRenderer::PostRender(const prev::render::RenderContext& renderContext, const prev::render::DefaultRenderContextUserData& renderContextUserData)
{
}

void ShadowMapDebugRenderer::AfterRender(const prev::render::RenderContext& renderContext, const prev::render::DefaultRenderContextUserData& renderContextUserData)
{
}

void ShadowMapDebugRenderer::ShutDown()
{
    m_pipeline->ShutDown();
    m_pipeline = nullptr;

    m_shader->ShutDown();
    m_shader = nullptr;
}

void ShadowMapDebugRenderer::operator()(const prev::input::keyboard::KeyEvent& keyEvent)
{
    if (keyEvent.action == prev::input::keyboard::KeyActionType::PRESS) {
        if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_O) {
            m_cascadeIndex = (m_cascadeIndex - 1) < 0 ? prev_test::component::shadow::CASCADES_COUNT - 1 : m_cascadeIndex - 1;
            std::cout << "New Cascade Index = " << m_cascadeIndex << std::endl;
        } else if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_P) {
            m_cascadeIndex = (m_cascadeIndex + 1) % prev_test::component::shadow::CASCADES_COUNT;
            std::cout << "New Cascade Index = " << m_cascadeIndex << std::endl;
        }
    }
}
} // namespace prev_test::render::renderer::debug
