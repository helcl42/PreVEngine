#include "SelectionDebugRenderer.h"

#ifdef RENDER_SELECTION
#include "pipeline/SelectionDebugPipeline.h"
#include "shader/SelectionDebugShader.h"

#include "../../../component/ray_casting/ISelectableComponent.h"
#include "../../mesh/MeshFactory.h"
#include "../../model/ModelFactory.h"

#include <prev/core/DeviceProvider.h>
#include <prev/core/memory/buffer/UniformBuffer.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/AllocatorProvider.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::debug {
SelectionDebugRenderer::SelectionDebugRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void SelectionDebugRenderer::Init()
{
    auto device = prev::core::DeviceProvider::Instance().GetDevice();
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    prev::render::shader::ShaderFactory shaderFactory;
    m_shader = shaderFactory.CreateShaderFromFiles<shader::SelectionDebugShader>(*device, shader::SelectionDebugShader::GetPaths());
    m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Selection Debug Shader created\n");

    m_pipeline = std::make_unique<pipeline::SelectionDebugPipeline>(*device, *m_shader, *m_renderPass, m_renderPass->GetSamplesCount());
    m_pipeline->Init();

    LOGI("Selection Debug Pipeline created\n");

    m_uniformsPoolVS = std::make_unique<prev::core::memory::buffer::UBOPool<UniformsVS> >(*allocator);
    m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

    m_uniformsPoolFS = std::make_unique<prev::core::memory::buffer::UBOPool<UniformsFS> >(*allocator);
    m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh = meshFactory.CreateSphere(1.0f, 32, 32);
    m_selectionPointModel = CreateModel(*allocator, std::move(mesh));
}

void SelectionDebugRenderer::BeforeRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void SelectionDebugRenderer::PreRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
    const VkRect2D scissor{ { 0, 0 }, renderContext.fullExtent };
    const VkViewport viewport{ 0, 0, static_cast<float>(renderContextUserData.extent.width), static_cast<float>(renderContextUserData.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void SelectionDebugRenderer::Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const NormalRenderContextUserData& renderContextUserData)
{
    if (node->GetTags().HasAll({ TAG_SELECTABLE_COMPONENT })) {
        const auto selectableComponent = prev::scene::component::ComponentRepository<prev_test::component::ray_casting::ISelectableComponent>::Instance().Get(node->GetId());
        if (selectableComponent->IsSelected()) {
            auto uboVS = m_uniformsPoolVS->GetNext();

            UniformsVS uniformsVS{};
            uniformsVS.projectionMatrix = renderContextUserData.projectionMatrix;
            uniformsVS.viewMatrix = renderContextUserData.viewMatrix;
            uniformsVS.modelMatrix = prev::util::MathUtil::CreateTransformationMatrix(selectableComponent->GetPostiion(), glm::quat(), 0.6f);

            uboVS->Update(&uniformsVS);

            auto uboFS = m_uniformsPoolFS->GetNext();

            UniformsFS uniformsFS{};
            uniformsFS.color = glm::vec4(0.0f, 1.0f, 0.0f, 0.7f);

            uboFS->Update(&uniformsFS);

            m_shader->Bind("uboVS", *uboVS);
            m_shader->Bind("uboFS", *uboFS);

            const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
            const VkBuffer vertexBuffers[] = { *m_selectionPointModel->GetVertexBuffer() };
            const VkDeviceSize offsets[] = { 0 };

            vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.commandBuffer, *m_selectionPointModel->GetIndexBuffer(), 0, m_selectionPointModel->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.commandBuffer, m_selectionPointModel->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
        }
    }

    for (auto child : node->GetChildren()) {
        Render(renderContext, child, renderContextUserData);
    }
}

void SelectionDebugRenderer::PostRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void SelectionDebugRenderer::AfterRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void SelectionDebugRenderer::ShutDown()
{
    m_shader->ShutDown();

    m_pipeline->ShutDown();
}

std::unique_ptr<IModel> SelectionDebugRenderer::CreateModel(prev::core::memory::Allocator& allocator, const std::shared_ptr<IMesh>& mesh) const
{
    auto vertexBuffer = std::make_unique<prev::core::memory::buffer::VertexBuffer>(allocator);
    vertexBuffer->Data(mesh->GetVertexData(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());

    auto indexBuffer = std::make_unique<prev::core::memory::buffer::IndexBuffer>(allocator);
    indexBuffer->Data(mesh->GetIndices().data(), (uint32_t)mesh->GetIndices().size());

    prev_test::render::model::ModelFactory modelFactory{};
    return modelFactory.Create(mesh, std::move(vertexBuffer), std::move(indexBuffer));
}
} // namespace prev_test::render::renderer::debug

#endif