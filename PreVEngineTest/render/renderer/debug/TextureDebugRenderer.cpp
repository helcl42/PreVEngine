#include "TextureDebugRenderer.h"

#include "pipeline/TextureDebugPipeline.h"
#include "shader/TextureDebugShader.h"

#include "../../mesh/QuadMesh.h"
#include "../../model/ModelFactory.h"

#include "../../../Light.h"
#include "../../../RayCasting.h"
#include "../../../Water.h"

#include <prev/core/DeviceProvider.h>
#include <prev/core/memory/buffer/UniformBuffer.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/AllocatorProvider.h>

#include <memory>

namespace prev_test::render::renderer::debug {
TextureDebugRenderer::TextureDebugRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void TextureDebugRenderer::Init()
{
    auto device = prev::core::DeviceProvider::Instance().GetDevice();
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    prev::render::shader::ShaderFactory shaderFactory;
    m_shader = shaderFactory.CreateShaderFromFiles<prev_test::render::renderer::debug::shader::TextureDebugShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, AssetManager::Instance().GetAssetPath("Shaders/texture_debug_vert.spv") }, { VK_SHADER_STAGE_FRAGMENT_BIT, AssetManager::Instance().GetAssetPath("Shaders/texture_debug_frag.spv") } });
    m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Texture Debug Shader created\n");

    m_pipeline = std::make_unique<prev_test::render::renderer::debug::pipeline::TextureDebugPipeline>(*device, *m_renderPass, *m_shader);
    m_pipeline->Init();

    LOGI("Texture Debug Pipeline created\n");

    // create quad model
    auto quadMesh = std::make_unique<prev_test::render::mesh::QuadMesh>();

    auto vertexBuffer = std::make_unique<prev::core::memory::buffer::VertexBuffer>(*allocator);
    vertexBuffer->Data(quadMesh->GetVertexData(), quadMesh->GerVerticesCount(), quadMesh->GetVertexLayout().GetStride());

    auto indexBuffer = std::make_unique<prev::core::memory::buffer::IndexBuffer>(*allocator);
    indexBuffer->Data(quadMesh->GetIndices().data(), static_cast<uint32_t>(quadMesh->GetIndices().size()));

    m_quadModel = prev_test::render::model::ModelFactory{}.Create(std::move(quadMesh), std::move(vertexBuffer), std::move(indexBuffer));
}

void TextureDebugRenderer::BeforeRender(const prev::render::RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData)
{
}

void TextureDebugRenderer::PreRender(const prev::render::RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData)
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

void TextureDebugRenderer::Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode<SceneNodeFlags> >& node, const DefaultRenderContextUserData& renderContextUserData)
{
    const auto component = prev::scene::component::NodeComponentHelper::FindOne<SceneNodeFlags, IWaterOffscreenRenderPassComponent>(prev::common::FlagSet<SceneNodeFlags>{ SceneNodeFlags::WATER_REFLECTION_RENDER_COMPONENT });

    m_shader->Bind("imageSampler", component->GetColorImageBuffer()->GetImageView(), component->GetColorImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
    const VkBuffer vertexBuffers[] = { *m_quadModel->GetVertexBuffer() };
    const VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(renderContext.commandBuffer, *m_quadModel->GetIndexBuffer(), 0, m_quadModel->GetIndexBuffer()->GetIndexType());
    vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

    vkCmdDrawIndexed(renderContext.commandBuffer, m_quadModel->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
}

void TextureDebugRenderer::PostRender(const prev::render::RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData)
{
}

void TextureDebugRenderer::AfterRender(const prev::render::RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData)
{
}

void TextureDebugRenderer::ShutDown()
{
    m_shader->ShutDown();

    m_pipeline->ShutDown();
}
} // namespace prev_test::render::renderer::debug
