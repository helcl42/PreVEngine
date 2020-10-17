#include "ShadowMapDebugRenderer.h"

#include "pipeline/ShadowMapDebugPipeline.h"
#include "shader/ShadowMapDebugShader.h"

#include "../../../common/AssetManager.h"
#include "../../../component/shadow/IShadowsComponent.h"
#include "../../../component/shadow/ShadowsCommon.h"
#include "../../mesh/QuadMesh.h"
#include "../../model/ModelFactory.h"

#include <prev/core/DeviceProvider.h>
#include <prev/core/memory/buffer/UniformBuffer.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/AllocatorProvider.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::debug {
ShadowMapDebugRenderer::ShadowMapDebugRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void ShadowMapDebugRenderer::Init()
{
    auto device = prev::core::DeviceProvider::Instance().GetDevice();
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    prev::render::shader::ShaderFactory shaderFactory;
    m_shader = shaderFactory.CreateShaderFromFiles<prev_test::render::renderer::debug::shader::ShadowMapDebugShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/shadow_map_debug_vert.spv") }, { VK_SHADER_STAGE_FRAGMENT_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/shadow_map_debug_frag.spv") } });
    m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("ShadowMapDebug Shader created\n");

    m_pipeline = std::make_unique<prev_test::render::renderer::debug::pipeline::ShadowMapDebugPipeline>(*device, *m_renderPass, *m_shader);
    m_pipeline->Init();

    LOGI("ShadowMapDebug Pipeline created\n");

    // create quad model
    auto quadMesh = std::make_unique<prev_test::render::mesh::QuadMesh>();

    auto vertexBuffer = std::make_unique<prev::core::memory::buffer::VertexBuffer>(*allocator);
    vertexBuffer->Data(quadMesh->GetVertexData(), quadMesh->GerVerticesCount(), quadMesh->GetVertexLayout().GetStride());

    auto indexBuffer = std::make_unique<prev::core::memory::buffer::IndexBuffer>(*allocator);
    indexBuffer->Data(quadMesh->GetIndices().data(), static_cast<uint32_t>(quadMesh->GetIndices().size()));

    m_quadModel = prev_test::render::model::ModelFactory{}.Create(std::move(quadMesh), std::move(vertexBuffer), std::move(indexBuffer));
}

void ShadowMapDebugRenderer::BeforeRender(const prev::render::RenderContext& renderContext, const prev::render::DefaultRenderContextUserData& renderContextUserData)
{
}

void ShadowMapDebugRenderer::PreRender(const prev::render::RenderContext& renderContext, const prev::render::DefaultRenderContextUserData& renderContextUserData)
{
    VkRect2D renderRect{};
    renderRect.extent.width = renderContext.fullExtent.width / 2;
    renderRect.extent.height = renderContext.fullExtent.height / 2;
    renderRect.offset.x = 0;
    renderRect.offset.y = 0;

    VkRect2D scissor{};
    scissor.extent.width = renderContext.fullExtent.width;
    scissor.extent.height = renderContext.fullExtent.height;
    scissor.offset.x = 0;
    scissor.offset.y = 0;

    VkViewport viewport{};
    viewport.width = static_cast<float>(renderContext.fullExtent.width);
    viewport.height = static_cast<float>(renderContext.fullExtent.height);
    viewport.x = -static_cast<float>(renderContext.fullExtent.width / 2.0f);
    viewport.y = -static_cast<float>(renderContext.fullExtent.height / 2.0f);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

// make a node with quad model & shadowMap texture ???
void ShadowMapDebugRenderer::Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode<SceneNodeFlags> >& node, const prev::render::DefaultRenderContextUserData& renderContextUserData)
{
    const auto shadows = prev::scene::component::NodeComponentHelper::FindOne<SceneNodeFlags, prev_test::component::shadow::IShadowsComponent>({ TAG_SHADOW });

    const auto& cascade = shadows->GetCascade(static_cast<uint32_t>(m_cascadeIndex));
    PushConstantBlock pushConstBlock{ static_cast<uint32_t>(m_cascadeIndex), -cascade.startSplitDepth, -cascade.endSplitDepth };
    vkCmdPushConstants(renderContext.commandBuffer, m_pipeline->GetLayout(), VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantBlock), &pushConstBlock);

    m_shader->Bind("depthSampler", shadows->GetImageBuffer()->GetImageView(), shadows->GetImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);

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
    m_shader->ShutDown();

    m_pipeline->ShutDown();
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
