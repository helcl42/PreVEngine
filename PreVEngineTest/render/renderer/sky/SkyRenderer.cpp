#include "SkyRenderer.h"

#include "pipeline/SkyCompositePipeline.h"
#include "pipeline/SkyPipeline.h"
#include "pipeline/SkyPostProcessPipeline.h"

#include "shader/SkyCompositeShader.h"
#include "shader/SkyPostProcessShader.h"
#include "shader/SkyShader.h"

#include "../../../component/cloud/ICloudsComponent.h"
#include "../../../component/light/ILightComponent.h"
#include "../../../component/sky/ISkyComponent.h"
#include "../../../component/time/ITimeComponent.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/render/buffer/image/ImageBufferFactory.h>
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

    LOGI("Sky Compute Shader created\n");

    m_skyPipeline = std::make_unique<pipeline::SkyPipeline>(*device, *m_skyShader);
    m_skyPipeline->Init();

    LOGI("Sky Compute Pipeline created\n");

    m_uniformsPoolSkyCS = std::make_unique<prev::render::buffer::UniformBufferRing<UniformsSkyCS>>(*allocator);
    m_uniformsPoolSkyCS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));

    // compute sky post process
    m_skyPostProcessShader = shaderFactory.CreateShaderFromFiles<shader::SkyPostProcessShader>(*device, shader::SkyPostProcessShader::GetPaths());
    m_skyPostProcessShader->Init();
    m_skyPostProcessShader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Sky PostProcess Compute Shader created\n");

    m_skyPostProcessPipeline = std::make_unique<pipeline::SkyPostProcessPipeline>(*device, *m_skyPostProcessShader);
    m_skyPostProcessPipeline->Init();

    LOGI("Sky PostProcess Compute Pipeline created\n");

    m_uniformsPoolSkyPostProcessCS = std::make_unique<prev::render::buffer::UniformBufferRing<UniformsSkyPostProcessCS>>(*allocator);
    m_uniformsPoolSkyPostProcessCS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));

    // compositor
    m_compositeShader = shaderFactory.CreateShaderFromFiles<shader::SkyCompositeShader>(*device, shader::SkyCompositeShader::GetPaths());
    m_compositeShader->Init();
    m_compositeShader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Sky Composite Shader created\n");

    m_compositePipeline = std::make_unique<pipeline::SkyCompositePipeline>(*device, *m_compositeShader, *m_renderPass);
    m_compositePipeline->Init();

    LOGI("Sky Composite Pipeline created\n");
}

void SkyRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
    const auto skyComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::sky::ISkyComponent>({ TAG_SKY_RENDER_COMPONENT });
    const auto cloudsComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::cloud::ICloudsComponent>({ TAG_CLOUDS_COMPONENT });
    const auto mainLightComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::light::ILightComponent>({ TAG_MAIN_LIGHT });
    const auto timeComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::time::ITimeComponent>({ TAG_TIME_COMPONENT });

    // generate clouds usgin compute queue
    const VkExtent2D extent{ renderContext.rect.extent.width - renderContext.rect.offset.x, renderContext.rect.extent.height - renderContext.rect.offset.y };

    // TODO - put this work on dedicated compute queue if available, double buffering, ...

    UpdateImageBufferExtents(extent, COLOR_FORMAT, m_skyColorImageBuffer, m_skyColorImageSampler);
    UpdateImageBufferExtents(extent, COLOR_FORMAT, m_skyBloomImageBuffer, m_skyBloomImageSampler);
    UpdateImageBufferExtents(extent, COLOR_FORMAT, m_skyAlphanessImageBuffer, m_skyAlphanessImageSampler);
    UpdateImageBufferExtents(extent, DEPTH_FORMAT, m_skyCloudDistanceImageBuffer, m_skyCloudDistanceImageSampler);
    UpdateImageBufferExtents(extent, COLOR_FORMAT, m_skyPostProcessColorImageBuffer, m_skyPostProcessImageSampler);

    // regular sky render
    AddImageBufferPipelineBarrierCommand(m_skyColorImageBuffer->GetImage(), VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, renderContext.commandBuffer);
    AddImageBufferPipelineBarrierCommand(m_skyBloomImageBuffer->GetImage(), VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, renderContext.commandBuffer);
    AddImageBufferPipelineBarrierCommand(m_skyAlphanessImageBuffer->GetImage(), VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, renderContext.commandBuffer);
    AddImageBufferPipelineBarrierCommand(m_skyCloudDistanceImageBuffer->GetImage(), VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, renderContext.commandBuffer);

    auto uboCS = m_uniformsPoolSkyCS->GetNext();

    UniformsSkyCS uniformsCS{};
    uniformsCS.resolution = glm::vec4(extent.width, extent.height, 0.0f, 0.0f);
    uniformsCS.projectionMatrix = renderContext.projectionMatrix;
    uniformsCS.inverseProjectionMatrix = glm::inverse(renderContext.projectionMatrix);
    uniformsCS.viewMatrix = renderContext.viewMatrix;
    uniformsCS.inverseViewMatrix = glm::inverse(renderContext.viewMatrix);
    uniformsCS.lightColor = glm::vec4(mainLightComponent->GetColor(), 1.0f);
    uniformsCS.lightDirection = glm::vec4(-mainLightComponent->GetDirection(), 0.0f);
    uniformsCS.cameraPosition = glm::vec4(renderContext.cameraPosition, 1.0f);
    uniformsCS.baseCloudColor = cloudsComponent->GetColor();
    uniformsCS.skyColorBottom = glm::vec4(skyComponent->GetBottomColor(), 1.0f);
    uniformsCS.skyColorTop = glm::vec4(skyComponent->GetTopColor(), 1.0f);
    uniformsCS.windDirection = glm::normalize(glm::vec4(0.5f, 0.0f, 0.1f, 0.0f));
    uniformsCS.time = timeComponent->GetElapsedTime();
    uniformsCS.coverageFactor = 0.45f;
    uniformsCS.cloudSpeed = 450.0f;
    uniformsCS.crispiness = 40.0f;
    uniformsCS.absorption = 0.0035f;
    uniformsCS.curliness = 0.1f;
    uniformsCS.enablePowder = 0;
    uniformsCS.densityFactor = 0.02f;
    uniformsCS.earthRadius = 600000.0f;
    uniformsCS.sphereInnerRadius = uniformsCS.earthRadius + 5000.0f;
    uniformsCS.sphereOuterRadius = uniformsCS.sphereInnerRadius + 17000.0f;
    uniformsCS.cloudTopOffset = 750.0f;

    uboCS->Update(&uniformsCS);

    m_skyShader->Bind("uboCS", *uboCS);

    m_skyShader->Bind("perlinNoiseTex", cloudsComponent->GetPerlinWorleyNoise()->GetImageView(), *cloudsComponent->GetPerlinWorleyNoiseSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    m_skyShader->Bind("weatherTex", cloudsComponent->GetWeather()->GetImageView(), *cloudsComponent->GetWeatherSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    m_skyShader->Bind("outFragColor", m_skyColorImageBuffer->GetImageView(), *m_skyColorImageSampler, VK_IMAGE_LAYOUT_GENERAL);
    m_skyShader->Bind("outBloom", m_skyBloomImageBuffer->GetImageView(), *m_skyBloomImageSampler, VK_IMAGE_LAYOUT_GENERAL);
    m_skyShader->Bind("outAlphaness", m_skyAlphanessImageBuffer->GetImageView(), *m_skyAlphanessImageSampler, VK_IMAGE_LAYOUT_GENERAL);
    m_skyShader->Bind("outCloudDistance", m_skyCloudDistanceImageBuffer->GetImageView(), *m_skyCloudDistanceImageSampler, VK_IMAGE_LAYOUT_GENERAL);

    const VkDescriptorSet descriptorSetCompute = m_skyShader->UpdateNextDescriptorSet();

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, *m_skyPipeline);
    vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_skyPipeline->GetLayout(), 0, 1, &descriptorSetCompute, 0, 0);

    vkCmdDispatch(renderContext.commandBuffer, prev::util::vk::GetComputeGroupSize(extent.width, 16), prev::util::vk::GetComputeGroupSize(extent.height, 16), 1);

    AddImageBufferPipelineBarrierCommand(m_skyColorImageBuffer->GetImage(), VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, renderContext.commandBuffer);
    AddImageBufferPipelineBarrierCommand(m_skyBloomImageBuffer->GetImage(), VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, renderContext.commandBuffer);
    AddImageBufferPipelineBarrierCommand(m_skyAlphanessImageBuffer->GetImage(), VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, renderContext.commandBuffer);
    AddImageBufferPipelineBarrierCommand(m_skyCloudDistanceImageBuffer->GetImage(), VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, renderContext.commandBuffer);

    // sky post process render
    AddImageBufferPipelineBarrierCommand(m_skyPostProcessColorImageBuffer->GetImage(), VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, renderContext.commandBuffer);

    auto uboPostCS = m_uniformsPoolSkyPostProcessCS->GetNext();

    glm::vec4 lightPositionClipSpace = renderContext.projectionMatrix * renderContext.viewMatrix * glm::vec4(mainLightComponent->GetPosition(), 1.0f);
    glm::vec3 lightPositionNdc = lightPositionClipSpace / lightPositionClipSpace.w;
    glm::vec2 lightPositionO1 = lightPositionNdc * 0.5f + 0.5f;

    UniformsSkyPostProcessCS uniformsPostCS{};
    uniformsPostCS.resolution = glm::vec4(extent.width, extent.height, 0.0f, 0.0f);
    uniformsPostCS.lisghtPosition = glm::vec4(lightPositionO1, 0.0f, 1.0f);
    uniformsPostCS.enableGodRays = 1;
    uniformsPostCS.lightDotCameraForward = glm::dot(glm::normalize(renderContext.cameraPosition - mainLightComponent->GetPosition()), glm::normalize(prev::util::math::GetForwardVector(renderContext.viewMatrix)));

    uboPostCS->Update(&uniformsPostCS);

    m_skyPostProcessShader->Bind("uboCS", *uboPostCS);

    m_skyPostProcessShader->Bind("skyTex", m_skyColorImageBuffer->GetImageView(), *m_skyColorImageSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    m_skyPostProcessShader->Bind("bloomTex", m_skyBloomImageBuffer->GetImageView(), *m_skyBloomImageSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    m_skyPostProcessShader->Bind("outFragColor", m_skyPostProcessColorImageBuffer->GetImageView(), *m_skyPostProcessImageSampler, VK_IMAGE_LAYOUT_GENERAL);

    const VkDescriptorSet descriptorSetComputePost = m_skyPostProcessShader->UpdateNextDescriptorSet();

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, *m_skyPostProcessPipeline);
    vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_skyPostProcessPipeline->GetLayout(), 0, 1, &descriptorSetComputePost, 0, 0);

    vkCmdDispatch(renderContext.commandBuffer, prev::util::vk::GetComputeGroupSize(extent.width, 16), prev::util::vk::GetComputeGroupSize(extent.height, 16), 1);

    AddImageBufferPipelineBarrierCommand(m_skyPostProcessColorImageBuffer->GetImage(), VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, renderContext.commandBuffer);
}

void SkyRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_compositePipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void SkyRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (node->GetTags().HasAll({ TAG_SKY_RENDER_COMPONENT })) {
        const auto skyComponent = prev::scene::component::ComponentRepository<prev_test::component::sky::ISkyComponent>::Instance().Get(node->GetId());

        m_compositeShader->Bind("colorTex", m_skyPostProcessColorImageBuffer->GetImageView(), *m_skyPostProcessImageSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        m_compositeShader->Bind("depthTex", m_skyCloudDistanceImageBuffer->GetImageView(), *m_skyCloudDistanceImageSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        const VkDescriptorSet descriptorSet = m_compositeShader->UpdateNextDescriptorSet();
        const VkBuffer vertexBuffers[] = { *skyComponent->GetModel()->GetVertexBuffer() };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(renderContext.commandBuffer, *skyComponent->GetModel()->GetIndexBuffer(), 0, skyComponent->GetModel()->GetIndexBuffer()->GetIndexType());
        vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_compositePipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(renderContext.commandBuffer, skyComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
    }
}

void SkyRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void SkyRenderer::AfterRender(const NormalRenderContext& renderContext)
{
}

void SkyRenderer::ShutDown()
{
    m_skyPostProcessColorImageBuffer = nullptr;
    m_skyCloudDistanceImageBuffer = nullptr;
    m_skyAlphanessImageBuffer = nullptr;
    m_skyBloomImageBuffer = nullptr;
    m_skyColorImageBuffer = nullptr;

    m_compositePipeline->ShutDown();
    m_compositePipeline = nullptr;

    m_compositeShader->ShutDown();
    m_compositeShader = nullptr;

    m_skyPostProcessPipeline->ShutDown();
    m_skyPostProcessPipeline = nullptr;

    m_skyPostProcessShader->ShutDown();
    m_skyPostProcessShader = nullptr;

    m_skyPipeline->ShutDown();
    m_skyPipeline = nullptr;

    m_skyShader->ShutDown();
    m_skyShader = nullptr;
}

void SkyRenderer::UpdateImageBufferExtents(const VkExtent2D& extent, const VkFormat format, std::shared_ptr<prev::render::buffer::image::IImageBuffer>& imageBuffer, std::shared_ptr<prev::render::sampler::Sampler>& sampler)
{
    if (imageBuffer == nullptr || imageBuffer->GetExtent().width != extent.width || imageBuffer->GetExtent().height != extent.height) {
        auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

        imageBuffer = prev::render::buffer::image::ImageBufferFactory{}.CreateStorage(prev::render::buffer::image::ImageBufferCreateInfo{ VkExtent2D{ extent.width, extent.height }, VK_IMAGE_TYPE_2D, format, VK_SAMPLE_COUNT_1_BIT, 0, false, VK_IMAGE_VIEW_TYPE_2D, 1 }, *allocator);
        allocator->TransitionImageLayout(imageBuffer->GetImage(), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, imageBuffer->GetFormat(), imageBuffer->GetMipLevels(), imageBuffer->GetLayerCount());

        sampler = std::make_shared<prev::render::sampler::Sampler>(allocator->GetDevice(), static_cast<float>(imageBuffer->GetMipLevels()), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR);
    }
}

void SkyRenderer::AddImageBufferPipelineBarrierCommand(const VkImage image, const VkAccessFlags srcAccessMask, const VkAccessFlags dstAccessMask, const VkImageLayout oldLayout, const VkImageLayout newLayout, const VkPipelineStageFlags srcShaderStageMask, const VkPipelineStageFlags dstShaderStageMask, VkCommandBuffer commandBuffer)
{
    VkImageMemoryBarrier barrierDescription{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
    barrierDescription.srcAccessMask = srcAccessMask,
    barrierDescription.dstAccessMask = dstAccessMask;
    barrierDescription.oldLayout = oldLayout;
    barrierDescription.newLayout = newLayout;
    barrierDescription.image = image;
    barrierDescription.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    barrierDescription.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierDescription.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    vkCmdPipelineBarrier(commandBuffer, srcShaderStageMask, dstShaderStageMask, 0, 0, nullptr, 0, nullptr, 1, &barrierDescription);
}
} // namespace prev_test::render::renderer::sky
