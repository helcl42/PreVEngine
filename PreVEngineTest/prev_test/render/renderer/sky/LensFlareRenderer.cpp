#include "LensFlareRenderer.h"

#include "../../../common/AssetManager.h"
#include "../../../component/light/ILightComponent.h"
#include "../../../component/sky/ILensFlareComponent.h"

#include <prev/render/pipeline/PipelineBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::sky {
LensFlareRenderer::LensFlareRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass)
    : m_device{ device }
    , m_allocator{ allocator }
    , m_renderPass{ renderPass }
{
}

void LensFlareRenderer::Init()
{
    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/sky/lens_flare_vert.spv") },
            { VK_SHADER_STAGE_FRAGMENT_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/sky/lens_flare_frag.spv") }
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
            { "uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT },
            { "uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT },
            { "colorSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT },
        })
        .SetDescriptorPoolCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("LensFlare Shader created");

    // clang-format off
    m_pipeline = prev::render::pipeline::GraphicsPipelineBuilder{ m_device, *m_shader, m_renderPass }
        .SetPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetDepthTestEnabled(true)
        .SetDepthWriteEnabled(true)
        .SetBlendingModeEnabled(true)
        .SetAdditiveBlendingEnabled(false)
        .SetPolygonMode(VK_POLYGON_MODE_FILL)
        .SetCullingMode(VK_CULL_MODE_BACK_BIT)
        .Build();
    // clang-format on

    LOGI("LensFlare Pipeline created");

    m_uniformsPoolVS = std::make_unique<prev::render::buffer::UniformRingBuffer<UniformsVS>>(m_allocator);
    m_uniformsPoolVS->UpdateCapacity(m_descriptorCount, static_cast<uint32_t>(m_device.GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

    m_uniformsPoolFS = std::make_unique<prev::render::buffer::UniformRingBuffer<UniformsFS>>(m_allocator);
    m_uniformsPoolFS->UpdateCapacity(m_descriptorCount, static_cast<uint32_t>(m_device.GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
}

void LensFlareRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
}

void LensFlareRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void LensFlareRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (node->GetTags().HasAll({ TAG_LENS_FLARE_RENDER_COMPONENT })) {
        const auto lensFlareComponent{ prev::scene::component::ComponentRepository<prev_test::component::sky::ILensFlareComponent>::Instance().Get(node->GetId()) };
        const auto lightComponent{ prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::light::ILightComponent>({ TAG_MAIN_LIGHT }) };

        std::vector<glm::vec2> flarePositions[MAX_VIEW_COUNT];
        for (uint32_t viewIndex = 0; viewIndex < renderContext.cameraCount; ++viewIndex) {
            flarePositions[viewIndex] = lensFlareComponent->ComputeFlarePositions(renderContext.projectionMatrices[viewIndex], renderContext.viewMatrices[viewIndex], renderContext.cameraPositions[viewIndex], lightComponent->GetPosition());
        }

        const auto& flares{ lensFlareComponent->GetFlares() };
        for (size_t i = 0; i < flares.size(); ++i) {
            const auto& flare{ flares[i] };
            const float aspectRatio{
                static_cast<float>(renderContext.rect.extent.width - renderContext.rect.offset.x) / static_cast<float>(renderContext.rect.extent.height - renderContext.rect.offset.y)
            };
            const float xScale{ flare->GetScale() };
            const float yScale{ xScale * aspectRatio };

            auto uboVS = m_uniformsPoolVS->GetNext();
            UniformsVS uniformsVS{};
            for (uint32_t viewIndex = 0; viewIndex < renderContext.cameraCount; ++viewIndex) {
                uniformsVS.translations[viewIndex] = glm::vec4(flarePositions[viewIndex][i], MIN_DEPTH, 1.0f);
            }
            uniformsVS.scale = glm::vec4(xScale, yScale, 0.0f, 0.0f);
            uboVS->Update(&uniformsVS);

            auto uboFS = m_uniformsPoolFS->GetNext();
            UniformsFS uniformsFS{};
            uniformsFS.brightness = glm::vec4(m_sunVisibilityFactor);
            uboFS->Update(&uniformsFS);

            m_shader->Bind("colorSampler", *flare->GetImageBuffer(), *flare->GetSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            m_shader->Bind("uboVS", *uboVS);
            m_shader->Bind("uboFS", *uboFS);

            const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
            const VkBuffer vertexBuffers[] = { *lensFlareComponent->GetModel()->GetVertexBuffer() };
            const VkDeviceSize offsets[] = { 0 };

            vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.commandBuffer, *lensFlareComponent->GetModel()->GetIndexBuffer(), 0, lensFlareComponent->GetModel()->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.commandBuffer, lensFlareComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
        }
    }
}

void LensFlareRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void LensFlareRenderer::AfterRender(const NormalRenderContext& renderContext)
{
}

void LensFlareRenderer::ShutDown()
{
    m_pipeline = nullptr;
    m_shader = nullptr;
}

void LensFlareRenderer::operator()(const prev_test::render::renderer::sky::SunVisibilityEvent& evt)
{
    m_sunVisibilityFactor = evt.visibilityFactor;
}
} // namespace prev_test::render::renderer::sky
