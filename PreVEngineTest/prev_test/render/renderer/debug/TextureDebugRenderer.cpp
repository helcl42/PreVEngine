#include "TextureDebugRenderer.h"

#include "../../../common/AssetManager.h"
#include "../../../component/common/IOffScreenRenderPassComponent.h"
#include "../../mesh/MeshFactory.h"
#include "../../model/ModelFactory.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/render/pipeline/PipelineBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::debug {
TextureDebugRenderer::TextureDebugRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void TextureDebugRenderer::Init()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ *device }
        .AddShaderStagePaths({
            { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/debug/texture_debug_vert.spv") },
            { VK_SHADER_STAGE_FRAGMENT_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/debug/texture_debug_frag.spv") }
        })
        .AddVertexInputAttributeDescriptions({
            prev::util::vk::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 }))
        })
        .AddVertexInputBindingDescriptions({
            prev::util::vk::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
        })
        .AddDescriptorSets({
            { "imageSampler", 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT }
        })
	    .SetDescriptorPoolCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Texture Debug Shader created\n");

    // clang-format off
    m_pipeline = prev::render::pipeline::GraphicsPipelineBuilder{ *device, *m_shader, *m_renderPass }
        .SetPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetDepthTestEnabled(false)
        .SetDepthWriteEnabled(false)
        .SetBlendingModeEnabled(false)
        .SetAdditiveBlendingEnabled(false)
        .SetPolygonMode(VK_POLYGON_MODE_LINE)
        .Build();
    // clang-format on

    LOGI("Texture Debug Pipeline created\n");

    // create quad model
    prev_test::render::mesh::MeshFactory meshFactory{};
    auto quadMesh{ meshFactory.CreateQuad() };

    prev_test::render::model::ModelFactory modelFactory{};
    m_quadModel = modelFactory.Create(std::move(quadMesh), *allocator);
}

void TextureDebugRenderer::BeforeRender(const prev::render::RenderContext& renderContext)
{
}

void TextureDebugRenderer::PreRender(const prev::render::RenderContext& renderContext)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void TextureDebugRenderer::Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    const auto component = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::common::IOffScreenRenderPassComponent>({ TAG_WATER_REFLECTION_RENDER_COMPONENT });

    m_shader->Bind("imageSampler", *component->GetColorImageBuffer(), *component->GetColorSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
    const VkBuffer vertexBuffers[] = { *m_quadModel->GetVertexBuffer() };
    const VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(renderContext.commandBuffer, *m_quadModel->GetIndexBuffer(), 0, m_quadModel->GetIndexBuffer()->GetIndexType());
    vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

    vkCmdDrawIndexed(renderContext.commandBuffer, m_quadModel->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
}

void TextureDebugRenderer::PostRender(const prev::render::RenderContext& renderContext)
{
}

void TextureDebugRenderer::AfterRender(const prev::render::RenderContext& renderContext)
{
}

void TextureDebugRenderer::ShutDown()
{
    m_pipeline = nullptr;
    m_shader = nullptr;
}
} // namespace prev_test::render::renderer::debug
