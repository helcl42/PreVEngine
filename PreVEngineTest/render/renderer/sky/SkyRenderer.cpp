#include "SkyRenderer.h"

#include "pipeline/SkyCompositePipeline.h"
#include "pipeline/SkyPipeline.h"
#include "pipeline/SkyPostProcessPipeline.h"

#include "shader/SkyCompositeShader.h"
#include "shader/SkyPostProcessShader.h"
#include "shader/SkyShader.h"

#include "../../../component/cloud/ICloudsComponent.h"
#include "../../../component/common/OffScreenRenderPassComponentFactory.h"
#include "../../../component/light/ILightComponent.h"
#include "../../../component/sky/ISkyComponent.h"
#include "../../../component/time/ITimeComponent.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/render/buffer/image/ImageStorageBuffer.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::sky {
SkyRenderer::SkyRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void SkyRenderer::Init()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev::render::shader::ShaderFactory shaderFactory;

    // compute sky
    m_skyShader = shaderFactory.CreateShaderFromFiles<shader::SkyShader>(*device, shader::SkyShader::GetPaths());
    m_skyShader->Init();
    m_skyShader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Sky Shader created\n");

    m_uniformsPoolSkyFS = std::make_unique<prev::render::buffer::UBOPool<UniformsSkyFS>>(*allocator);
    m_uniformsPoolSkyFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));

    // compute sky post process
    m_skyPostProcessShader = shaderFactory.CreateShaderFromFiles<shader::SkyPostProcessShader>(*device, shader::SkyPostProcessShader::GetPaths());
    m_skyPostProcessShader->Init();
    m_skyPostProcessShader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Sky PostProcess Shader created\n");

    m_uniformsPoolSkyPostProcessFS = std::make_unique<prev::render::buffer::UBOPool<UniformsSkyPostProcessFS>>(*allocator);
    m_uniformsPoolSkyPostProcessFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));

    // compositor
    m_skyCompositeShader = shaderFactory.CreateShaderFromFiles<shader::SkyCompositeShader>(*device, shader::SkyCompositeShader::GetPaths());
    m_skyCompositeShader->Init();
    m_skyCompositeShader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Sky Composite Shader created\n");

    m_skyCompositePipeline = std::make_unique<pipeline::SkyCompositePipeline>(*device, *m_skyCompositeShader, *m_renderPass);
    m_skyCompositePipeline->Init();

    LOGI("Sky Composite Pipeline created\n");
}

void SkyRenderer::BeforeRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    const auto skyComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::sky::ISkyComponent>({ TAG_SKY_RENDER_COMPONENT });
    const auto cloudsComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::cloud::ICloudsComponent>({ TAG_CLOUDS_COMPONENT });
    const auto mainLightComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::light::ILightComponent>({ TAG_MAIN_LIGHT });
    const auto timeComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::time::ITimeComponent>({ TAG_TIME_COMPONENT });

    const VkExtent2D extent{ renderContext.rect.extent.width - renderContext.rect.offset.x, renderContext.rect.extent.height - renderContext.rect.offset.y };

    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    // sky rendering
    {
        if (UpdateOffScreenRenderTarget(extent, m_skyOffScreenRenderTargetDepthFormat, m_skyOffScreenRenderTargetColorFormats, m_skyOffScreenRenderTarget)) {
            if (m_skyPipeline) {
                m_skyPipeline->ShutDown();
                m_skyPipeline = nullptr;
            }
            m_skyPipeline = std::make_unique<pipeline::SkyPipeline>(*device, *m_skyShader, *m_skyOffScreenRenderTarget->GetRenderPass());
            m_skyPipeline->Init();
            LOGI("Sky Pipeline created\n");
        }

        m_skyOffScreenRenderTarget->GetRenderPass()->Begin(m_skyOffScreenRenderTarget->GetFrameBuffer(), renderContext.commandBuffer, renderContext.rect);

        vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_skyPipeline);
        vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);

        auto uboFS{ m_uniformsPoolSkyFS->GetNext() };

        UniformsSkyFS uniformsFS{};
        uniformsFS.resolution = glm::vec4(extent.width, extent.height, 0.0f, 0.0f);
        uniformsFS.projectionMatrix = renderContextUserData.projectionMatrix;
        uniformsFS.inverseProjectionMatrix = glm::inverse(renderContextUserData.projectionMatrix);
        uniformsFS.viewMatrix = renderContextUserData.viewMatrix;
        uniformsFS.inverseViewMatrix = glm::inverse(renderContextUserData.viewMatrix);
        uniformsFS.lightColor = glm::vec4(mainLightComponent->GetColor(), 1.0f);
        uniformsFS.lightDirection = glm::vec4(-mainLightComponent->GetDirection(), 0.0f);
        uniformsFS.cameraPosition = glm::vec4(renderContextUserData.cameraPosition, 1.0f);
        uniformsFS.baseCloudColor = cloudsComponent->GetColor();
        uniformsFS.skyColorBottom = glm::vec4(skyComponent->GetBottomColor(), 1.0f);
        uniformsFS.skyColorTop = glm::vec4(skyComponent->GetTopColor(), 1.0f);
        uniformsFS.windDirection = glm::vec4(0.5f, 0.0f, 0.1f, 0.0f);
        uniformsFS.time = timeComponent->GetElapsedTime();
        uniformsFS.coverageFactor = 0.45f;
        uniformsFS.cloudSpeed = 450.0f;
        uniformsFS.crispiness = 40.0f;
        uniformsFS.absorption = 0.0035f;
        uniformsFS.curliness = 0.1f;
        uniformsFS.enablePowder = 0;
        uniformsFS.densityFactor = 0.02f;
        uniformsFS.earthRadius = 600000.0f;
        uniformsFS.sphereInnerRadius = uniformsFS.earthRadius + 5000.0f;
        uniformsFS.sphereOuterRadius = uniformsFS.sphereInnerRadius + 17000.0f;
        uniformsFS.cloudTopOffset = 750.0f;

        uboFS->Update(&uniformsFS);

        m_skyShader->Bind("uboFS", *uboFS);
        m_skyShader->Bind("perlinNoiseTex", cloudsComponent->GetPerlinWorleyNoise()->GetImageView(), *cloudsComponent->GetPerlinWorleyNoiseSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        m_skyShader->Bind("weatherTex", cloudsComponent->GetWeather()->GetImageView(), *cloudsComponent->GetWeatherSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        const VkDescriptorSet descriptorSet = m_skyShader->UpdateNextDescriptorSet();
        const VkBuffer vertexBuffers[] = { *cloudsComponent->GetModel()->GetVertexBuffer() };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(renderContext.commandBuffer, *cloudsComponent->GetModel()->GetIndexBuffer(), 0, cloudsComponent->GetModel()->GetIndexBuffer()->GetIndexType());
        vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_skyPipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(renderContext.commandBuffer, cloudsComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);

        m_skyOffScreenRenderTarget->GetRenderPass()->End(renderContext.commandBuffer);
    }

    // sky post process render
    {

        if (UpdateOffScreenRenderTarget(extent, m_skyPostProcessOffScreenRenderTargetDepthFormat, m_skyPostProcessOffScreenRenderTargetColorFormats, m_skyPostProcessOffScreenRenderTarget)) {
            if (m_skyPostProcessPipeline) {
                m_skyPostProcessPipeline->ShutDown();
                m_skyPostProcessPipeline = nullptr;
            }
            m_skyPostProcessPipeline = std::make_unique<pipeline::SkyPostProcessPipeline>(*device, *m_skyPostProcessShader, *m_skyPostProcessOffScreenRenderTarget->GetRenderPass());
            m_skyPostProcessPipeline->Init();
            LOGI("Sky PostProcess Pipeline created\n");
        }

        m_skyPostProcessOffScreenRenderTarget->GetRenderPass()->Begin(m_skyPostProcessOffScreenRenderTarget->GetFrameBuffer(), renderContext.commandBuffer, renderContext.rect);

        vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_skyPostProcessPipeline);
        vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);

        auto uboPostFS{ m_uniformsPoolSkyPostProcessFS->GetNext() };

        glm::vec4 lightPositionClipSpace = renderContextUserData.projectionMatrix * renderContextUserData.viewMatrix * glm::vec4(mainLightComponent->GetPosition(), 1.0f);
        glm::vec3 lightPositionNdc = lightPositionClipSpace / lightPositionClipSpace.w;
        glm::vec2 lightPositionO1 = lightPositionNdc * 0.5f + 0.5f;

        UniformsSkyPostProcessFS uniformsPostFS{};
        uniformsPostFS.resolution = glm::vec4(extent.width, extent.height, 0.0f, 0.0f);
        uniformsPostFS.lisghtPosition = glm::vec4(lightPositionO1, 0.0f, 1.0f);
        uniformsPostFS.enableGodRays = 1;
        uniformsPostFS.lightDotCameraForward = glm::dot(glm::normalize(renderContextUserData.cameraPosition - mainLightComponent->GetPosition()), glm::normalize(prev::util::math::GetForwardVector(renderContextUserData.viewMatrix)));

        uboPostFS->Update(&uniformsPostFS);

        m_skyPostProcessShader->Bind("uboFS", *uboPostFS);
        m_skyPostProcessShader->Bind("skyTex", m_skyOffScreenRenderTarget->GetColorImageBuffer(0)->GetImageView(), *m_skyOffScreenRenderTarget->GetColorSampler(0), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        m_skyPostProcessShader->Bind("bloomTex", m_skyOffScreenRenderTarget->GetColorImageBuffer(1)->GetImageView(), *m_skyOffScreenRenderTarget->GetColorSampler(1), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        const VkDescriptorSet descriptorSet = m_skyPostProcessShader->UpdateNextDescriptorSet();
        const VkBuffer vertexBuffers[] = { *cloudsComponent->GetModel()->GetVertexBuffer() };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(renderContext.commandBuffer, *cloudsComponent->GetModel()->GetIndexBuffer(), 0, cloudsComponent->GetModel()->GetIndexBuffer()->GetIndexType());
        vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_skyPostProcessPipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(renderContext.commandBuffer, cloudsComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);

        m_skyPostProcessOffScreenRenderTarget->GetRenderPass()->End(renderContext.commandBuffer);
    }
}

void SkyRenderer::PreRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_skyCompositePipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void SkyRenderer::Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const NormalRenderContextUserData& renderContextUserData)
{
    if (node->GetTags().HasAll({ TAG_SKY_RENDER_COMPONENT })) {
        const auto skyComponent = prev::scene::component::ComponentRepository<prev_test::component::sky::ISkyComponent>::Instance().Get(node->GetId());

        m_skyCompositeShader->Bind("colorTex", m_skyPostProcessOffScreenRenderTarget->GetColorImageBuffer(0)->GetImageView(), *m_skyPostProcessOffScreenRenderTarget->GetColorSampler(0), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        m_skyCompositeShader->Bind("depthTex", m_skyOffScreenRenderTarget->GetDepthImageBuffer()->GetImageView(), *m_skyOffScreenRenderTarget->GetDepthSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);

        const VkDescriptorSet descriptorSet = m_skyCompositeShader->UpdateNextDescriptorSet();
        const VkBuffer vertexBuffers[] = { *skyComponent->GetModel()->GetVertexBuffer() };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(renderContext.commandBuffer, *skyComponent->GetModel()->GetIndexBuffer(), 0, skyComponent->GetModel()->GetIndexBuffer()->GetIndexType());
        vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_skyCompositePipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(renderContext.commandBuffer, skyComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
    }

    for (auto& child : node->GetChildren()) {
        Render(renderContext, child, renderContextUserData);
    }
}

void SkyRenderer::PostRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void SkyRenderer::AfterRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void SkyRenderer::ShutDown()
{
    if (m_skyPostProcessOffScreenRenderTarget) {
        m_skyPostProcessOffScreenRenderTarget->ShutDown();
        m_skyPostProcessOffScreenRenderTarget = nullptr;
    }

    if (m_skyOffScreenRenderTarget) {
        m_skyOffScreenRenderTarget->ShutDown();
        m_skyOffScreenRenderTarget = nullptr;
    }

    if (m_skyPostProcessShader) {
        m_skyPostProcessShader->ShutDown();
        m_skyPostProcessShader = nullptr;
    }

    if (m_skyPostProcessPipeline) {
        m_skyPostProcessPipeline->ShutDown();
        m_skyPostProcessPipeline = nullptr;
    }

    m_skyCompositePipeline->ShutDown();
    m_skyCompositePipeline = nullptr;

    m_skyCompositeShader->ShutDown();
    m_skyCompositeShader = nullptr;

    m_skyPipeline->ShutDown();
    m_skyPipeline = nullptr;

    m_skyShader->ShutDown();
    m_skyShader = nullptr;
}

bool SkyRenderer::UpdateOffScreenRenderTarget(const VkExtent2D& extent, const VkFormat& depthFormat, const std::vector<VkFormat>& colorFormats, std::shared_ptr<prev_test::component::common::IOffScreenRenderPassComponent>& offScreenRenderTarget)
{
    if (!offScreenRenderTarget) {
        offScreenRenderTarget = prev_test::component::common::OffScreenRenderPassComponentFactory{}.Create(extent, depthFormat, colorFormats);
        offScreenRenderTarget->Init();
        return true;
    }

    const auto& renderTargetExtent{ offScreenRenderTarget->GetExtent() };
    if (renderTargetExtent.width != extent.width || renderTargetExtent.height != extent.height) {
        offScreenRenderTarget->ShutDown();
        offScreenRenderTarget = nullptr;

        offScreenRenderTarget = prev_test::component::common::OffScreenRenderPassComponentFactory{}.Create(extent, depthFormat, colorFormats);
        offScreenRenderTarget->Init();
        return true;
    }
    return false;
}
} // namespace prev_test::render::renderer::sky
