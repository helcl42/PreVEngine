#include "TextureDebugRenderer.h"
#include "pipeline/TextureDebugPipeline.h"
#include "shader/TextureDebugShader.h"

#include "../../../component/common/IOffScreenRenderPassComponent.h"
#include "../../mesh/MeshFactory.h"
#include "../../model/ModelFactory.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/core/memory/buffer/UniformBuffer.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::debug {
TextureDebugRenderer::TextureDebugRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void TextureDebugRenderer::Init()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev::render::shader::ShaderFactory shaderFactory;
    m_shader = shaderFactory.CreateShaderFromFiles<shader::TextureDebugShader>(*device, shader::TextureDebugShader::GetPaths());
    m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Texture Debug Shader created\n");

    m_pipeline = std::make_unique<pipeline::TextureDebugPipeline>(*device, *m_shader, *m_renderPass);
    m_pipeline->Init();

    LOGI("Texture Debug Pipeline created\n");

    // create quad model
    prev_test::render::mesh::MeshFactory meshFactory{};
    auto quadMesh{ meshFactory.CreateQuad() };

    prev_test::render::model::ModelFactory modelFactory{};
    m_quadModel = modelFactory.Create(std::move(quadMesh), *allocator);
}

void TextureDebugRenderer::BeforeRender(const prev::render::RenderContext& renderContext, const prev::render::DefaultRenderContextUserData& renderContextUserData)
{
}

void TextureDebugRenderer::PreRender(const prev::render::RenderContext& renderContext, const prev::render::DefaultRenderContextUserData& renderContextUserData)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void TextureDebugRenderer::Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const prev::render::DefaultRenderContextUserData& renderContextUserData)
{
    const auto component = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::common::IOffScreenRenderPassComponent>({ TAG_WATER_REFLECTION_RENDER_COMPONENT });

    m_shader->Bind("imageSampler", component->GetColorImageBuffer()->GetImageView(), component->GetColorImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
    const VkBuffer vertexBuffers[] = { *m_quadModel->GetVertexBuffer() };
    const VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(renderContext.commandBuffer, *m_quadModel->GetIndexBuffer(), 0, m_quadModel->GetIndexBuffer()->GetIndexType());
    vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

    vkCmdDrawIndexed(renderContext.commandBuffer, m_quadModel->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
}

void TextureDebugRenderer::PostRender(const prev::render::RenderContext& renderContext, const prev::render::DefaultRenderContextUserData& renderContextUserData)
{
}

void TextureDebugRenderer::AfterRender(const prev::render::RenderContext& renderContext, const prev::render::DefaultRenderContextUserData& renderContextUserData)
{
}

void TextureDebugRenderer::ShutDown()
{
    m_pipeline->ShutDown();
    m_pipeline = nullptr;

    m_shader->ShutDown();
    m_shader = nullptr;
}
} // namespace prev_test::render::renderer::debug
