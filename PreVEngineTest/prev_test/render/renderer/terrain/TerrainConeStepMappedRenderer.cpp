#include "TerrainConeStepMappedRenderer.h"

#include "../../../common/AssetManager.h"
#include "../../../component/light/ILightComponent.h"
#include "../../../component/ray_casting/IBoundingVolumeComponent.h"
#include "../../../component/ray_casting/RayCastingCommon.h"
#include "../../../component/shadow/IShadowsComponent.h"
#include "../../../component/sky/SkyCommon.h"
#include "../../../component/terrain/ITerrainComponent.h"
#include "../../../component/transform/ITransformComponent.h"

#include <prev/render/pipeline/PipelineBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::terrain {

constexpr uint32_t COLOR_INDEX{ 0 };
constexpr uint32_t NORMAL_INDEX{ 1 };
constexpr uint32_t HEIGHT_AND_CONE_INDEX{ 2 };

TerrainConeStepMappedRenderer::TerrainConeStepMappedRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass)
    : m_device{ device }
    , m_allocator{ allocator }
    , m_renderPass{ renderPass }
{
}

void TerrainConeStepMappedRenderer::Init()
{
    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/terrain/terrain_cone_step_mapped_vert.spv") },
            { VK_SHADER_STAGE_FRAGMENT_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/terrain/terrain_cone_step_mapped_frag.spv") }
        })
        .AddVertexInputAttributeDescriptions({
            prev::util::vk::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 })),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 4, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 })),
        })
        .AddVertexInputBindingDescriptions({
            prev::util::vk::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
        })
        .AddDescriptorSets({
            { "uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT },
            { "uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT },
            { "colorSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, VK_SHADER_STAGE_FRAGMENT_BIT },
            { "normalSampler", 3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, VK_SHADER_STAGE_FRAGMENT_BIT },
            { "heightSampler", 4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, VK_SHADER_STAGE_FRAGMENT_BIT },
            { "depthSampler", 5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT }
        })
	    .SetDescriptorPoolCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Terrain Cone Step Mapped Shader created\n");

    // clang-format off
    m_pipeline = prev::render::pipeline::GraphicsPipelineBuilder{ m_device, *m_shader, m_renderPass }
        .SetPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetDepthTestEnabled(true)
        .SetDepthWriteEnabled(true)
        .SetBlendingModeEnabled(true)
        .SetAdditiveBlendingEnabled(false)
        .SetPolygonMode(VK_POLYGON_MODE_FILL)
        .Build();
    // clang-format on

    LOGI("Terrain Cone Step Mapped Pipeline created\n");

    m_uniformsPoolVS = std::make_unique<prev::render::buffer::UniformBufferRing<UniformsVS>>(m_allocator);
    m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(m_device.GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));

    m_uniformsPoolFS = std::make_unique<prev::render::buffer::UniformBufferRing<UniformsFS>>(m_allocator);
    m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(m_device.GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));
}

void TerrainConeStepMappedRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
}

void TerrainConeStepMappedRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void TerrainConeStepMappedRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (node->GetTags().HasAll({ TAG_TERRAIN_CONE_STEP_MAPPED_RENDER_COMPONENT, TAG_TRANSFORM_COMPONENT })) {
        bool visible = true;
        if (prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
            visible = prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(renderContext.frustum);
        }

        if (visible) {
            const auto mainLightComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::light::ILightComponent>({ TAG_MAIN_LIGHT });
            const auto shadowsComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::shadow::IShadowsComponent>({ TAG_SHADOW });
            const auto lightComponents = prev::scene::component::NodeComponentHelper::FindAll<prev_test::component::light::ILightComponent>({ TAG_LIGHT });

            const auto transformComponent = prev::scene::component::ComponentRepository<prev_test::component::transform::ITransformComponent>::Instance().Get(node->GetId());
            const auto terrainComponent = prev::scene::component::ComponentRepository<prev_test::component::terrain::ITerrainComponenet>::Instance().Get(node->GetId());

            auto uboVS = m_uniformsPoolVS->GetNext();

            UniformsVS uniformsVS{};
            uniformsVS.projectionMatrix = renderContext.projectionMatrix;
            uniformsVS.viewMatrix = renderContext.viewMatrix;
            uniformsVS.modelMatrix = transformComponent->GetWorldTransformScaled();
            uniformsVS.normalMatrix = glm::transpose(glm::inverse(transformComponent->GetWorldTransformScaled()));
            uniformsVS.cameraPosition = glm::vec4(renderContext.cameraPosition, 1.0f);
            for (size_t i = 0; i < lightComponents.size(); i++) {
                const auto& lightComponent{ lightComponents[i] };
                uniformsVS.lightning.lights[i] = LightUniform(glm::vec4(lightComponent->GetPosition(), 1.0f), glm::vec4(lightComponent->GetColor(), 1.0f), glm::vec4(lightComponent->GetAttenuation(), 1.0f));
            }
            uniformsVS.lightning.realCountOfLights = static_cast<uint32_t>(lightComponents.size());
            uniformsVS.lightning.ambientFactor = prev_test::component::light::AMBIENT_LIGHT_INTENSITY;
            uniformsVS.density = prev_test::component::sky::FOG_DENSITY;
            uniformsVS.gradient = prev_test::component::sky::FOG_GRADIENT;
            uniformsVS.clipPlane = renderContext.clipPlane;

            uboVS->Update(&uniformsVS);

            auto uboFS = m_uniformsPoolFS->GetNext();

            UniformsFS uniformsFS{};
            // shadows
            for (size_t i = 0; i < prev_test::component::shadow::CASCADES_COUNT; i++) {
                const auto& cascade{ shadowsComponent->GetCascade(i) };
                uniformsFS.shadows.cascades[i] = ShadowsCascadeUniform(cascade.GetBiasedViewProjectionMatrix(), glm::vec4(cascade.endSplitDepth));
            }
            uniformsFS.shadows.enabled = prev_test::component::shadow::SHADOWS_ENABLED;
            uniformsFS.shadows.useReverseDepth = REVERSE_DEPTH;

            // lightning
            for (size_t i = 0; i < lightComponents.size(); i++) {
                const auto& lightComponent{ lightComponents[i] };
                uniformsFS.lightning.lights[i] = LightUniform(glm::vec4(lightComponent->GetPosition(), 1.0f), glm::vec4(lightComponent->GetColor(), 1.0f), glm::vec4(lightComponent->GetAttenuation(), 1.0f));
            }
            uniformsFS.lightning.realCountOfLights = static_cast<uint32_t>(lightComponents.size());
            uniformsFS.lightning.ambientFactor = prev_test::component::light::AMBIENT_LIGHT_INTENSITY;

            // common
            uniformsFS.fogColor = prev_test::component::sky::FOG_COLOR;
            uniformsFS.selectedColor = prev_test::component::ray_casting::SELECTED_COLOR;
            uniformsFS.selected = false;
            uniformsFS.castedByShadows = true;
            uniformsFS.minHeight = terrainComponent->GetHeightMapInfo()->GetGlobalMinHeight();
            uniformsFS.maxHeight = terrainComponent->GetHeightMapInfo()->GetGlobalMaxHeight();
            for (size_t i = 0; i < terrainComponent->GetMaterials().size(); i++) {
                const auto material{ terrainComponent->GetMaterials().at(i) };
                uniformsFS.material[i] = MaterialUniform(material->GetColor(), material->GetShineDamper(), material->GetReflectivity());
                uniformsFS.heightScale[i] = glm::vec4(material->GetHeightScale());
                uniformsFS.heightSteps[i] = glm::vec4(terrainComponent->GetHeightSteps().at(i));
            }
            uniformsFS.heightTransitionRange = terrainComponent->GetTransitionRange();
            uniformsFS.numLayers = 15;

            uboFS->Update(&uniformsFS);

            for (size_t i = 0; i < terrainComponent->GetMaterials().size(); i++) {
                const auto material{ terrainComponent->GetMaterials().at(i) };
                m_shader->Bind("colorSampler[" + std::to_string(i) + "]", *material->GetImageBuffer(COLOR_INDEX), *material->GetSampler(COLOR_INDEX), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                m_shader->Bind("normalSampler[" + std::to_string(i) + "]", *material->GetImageBuffer(NORMAL_INDEX), *material->GetSampler(NORMAL_INDEX), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                m_shader->Bind("heightSampler[" + std::to_string(i) + "]", *material->GetImageBuffer(HEIGHT_AND_CONE_INDEX), *material->GetSampler(HEIGHT_AND_CONE_INDEX), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            }
            m_shader->Bind("depthSampler", *shadowsComponent->GetImageBuffer(), *shadowsComponent->GetSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
            m_shader->Bind("uboVS", *uboVS);
            m_shader->Bind("uboFS", *uboFS);

            const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
            const VkBuffer vertexBuffers[] = { *terrainComponent->GetModel()->GetVertexBuffer() };
            const VkDeviceSize offsets[] = { 0 };

            vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.commandBuffer, *terrainComponent->GetModel()->GetIndexBuffer(), 0, terrainComponent->GetModel()->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.commandBuffer, terrainComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
        }
    }
}

void TerrainConeStepMappedRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void TerrainConeStepMappedRenderer::AfterRender(const NormalRenderContext& renderContext)
{
}

void TerrainConeStepMappedRenderer::ShutDown()
{
    m_pipeline = nullptr;
    m_shader = nullptr;
}
} // namespace prev_test::render::renderer::terrain
