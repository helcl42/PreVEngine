#include "Font3dRenderer.h"

#include "pipeline/Font3dPipeline.h"
#include "shader/Font3dShader.h"

#include "../../../component/font/IFontRenderComponent.h"
#include "../../../render/font/WorldSpaceText.h"

#include <prev/common/Common.h>
#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::font {
Font3dRenderer::Font3dRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void Font3dRenderer::Init()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev::render::shader::ShaderFactory shaderFactory;
    m_shader = shaderFactory.CreateShaderFromFiles<shader::Font3dShader>(*device, shader::Font3dShader::GetPaths());
    m_shader->Init();
    m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Fonts 3d Shader created\n");

    m_pipeline = std::make_unique<pipeline::Font3dPipeline>(*device, *m_shader, *m_renderPass);
    m_pipeline->Init();

    LOGI("Fonts 3d Pipeline created\n");

    m_uniformsPoolVS = std::make_unique<prev::render::buffer::UniformBufferRing<UniformsVS>>(*allocator);
    m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));

    m_uniformsPoolFS = std::make_unique<prev::render::buffer::UniformBufferRing<UniformsFS>>(*allocator);
    m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));
}

void Font3dRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
}

void Font3dRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void Font3dRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (node->GetTags().HasAll({ TAG_FONT_3D_RENDER_COMPONENT })) {
        const auto nodeFontRenderComponent = prev::scene::component::ComponentRepository<prev_test::component::font::IFontRenderComponent<prev_test::render::font::WorldSpaceText>>::Instance().Get(node->GetId());
        for (const auto& [key, renderableText] : nodeFontRenderComponent->GetRenderableTexts()) {
            auto uboVS = m_uniformsPoolVS->GetNext();
            UniformsVS uniformsVS{};
            uniformsVS.projectionMatrix = renderContext.projectionMatrix;
            uniformsVS.viewMatrix = renderContext.viewMatrix;
            uniformsVS.modelMatrix = prev::util::math::CreateTransformationMatrix(renderableText.text->GetPosition(), renderableText.text->IsAwaysFacingCamera() ? (glm::inverse(glm::quat_cast(renderContext.viewMatrix)) * renderableText.text->GetOrientation()) : (renderableText.text->GetOrientation() * glm::quat(glm::radians(glm::vec3(0.0f, 180.0f, 0.0f)))));
            uniformsVS.clipPlane = renderContext.clipPlane;
            uboVS->Update(&uniformsVS);

            auto uboFS = m_uniformsPoolFS->GetNext();
            UniformsFS uniformsFS{};
            uniformsFS.color = renderableText.text->GetColor();
            uniformsFS.width = glm::vec4(renderableText.text->GetWidth());
            uniformsFS.edge = glm::vec4(renderableText.text->GetEdge());
            uniformsFS.bias = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
            uniformsFS.borderWidth = glm::vec4(renderableText.text->GetBorderWidth());
            uniformsFS.borderEdge = glm::vec4(renderableText.text->GetBorderEdge());
            uniformsFS.hasEffect = renderableText.text->HasEffect() ? 1 : 0;
            uniformsFS.outlineColor = glm::vec4(renderableText.text->GetOutlineColor(), 1.0f);
            uniformsFS.outlineOffset = glm::vec4(renderableText.text->GetOutlineOffset(), 0.0f, 1.0f);
            uboFS->Update(&uniformsFS);

            m_shader->Bind("alphaSampler", nodeFontRenderComponent->GetFontMetadata()->GetImageBuffer()->GetImageView(), *nodeFontRenderComponent->GetFontMetadata()->GetSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            m_shader->Bind("uboVS", *uboVS);
            m_shader->Bind("uboFS", *uboFS);

            const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
            const VkBuffer vertexBuffers[] = { *renderableText.model->GetVertexBuffer() };
            const VkDeviceSize offsets[] = { 0 };

            vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.commandBuffer, *renderableText.model->GetIndexBuffer(), 0, renderableText.model->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.commandBuffer, renderableText.model->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
        }
    }
}

void Font3dRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void Font3dRenderer::AfterRender(const NormalRenderContext& renderContext)
{
}

void Font3dRenderer::ShutDown()
{
    m_pipeline->ShutDown();
    m_pipeline = nullptr;

    m_shader->ShutDown();
    m_shader = nullptr;
}
} // namespace prev_test::render::renderer::font
