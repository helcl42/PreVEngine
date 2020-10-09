#include "SkyRenderer.h"

#include "../common/pipeline/ScreenSpacePipeline.h"
#include "pipeline/SkyPipeline.h"
#include "pipeline/SkyPostProcessPipeline.h"

#include "../common/shader/ScreenSpaceShader.h"
#include "shader/SkyPostProcess.h"
#include "shader/SkyShader.h"

#include "../../../common/AssetManager.h"
#include "../../../component/time/ITimeComponent.h"

#include "../../../Clouds.h"
#include "../../../Sky.h"
#include "../../../component/light/ILightComponent.h"

#include <prev/core/DeviceProvider.h>
#include <prev/core/memory/buffer/UniformBuffer.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/AllocatorProvider.h>
#include <prev/scene/ComputeProvider.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>

#include <memory>

namespace prev_test::render::renderer::sky {
SkyRenderer::SkyRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void SkyRenderer::Init()
{
    auto device = prev::core::DeviceProvider::Instance().GetDevice();
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    prev::render::shader::ShaderFactory shaderFactory;

    // compute sky
    m_conmputeSkyShader = shaderFactory.CreateShaderFromFiles<prev_test::render::renderer::sky::shader::SkyShader>(*device, { { VK_SHADER_STAGE_COMPUTE_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/sky_comp.spv") } });
    m_conmputeSkyShader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Sky Compute Shader created\n");

    m_computeSkyPipeline = std::make_unique<prev_test::render::renderer::sky::pipeline::SkyPipeline>(*device, *m_conmputeSkyShader);
    m_computeSkyPipeline->Init();

    LOGI("Sky Compute Pipeline created\n");

    m_uniformsPoolSkyCS = std::make_unique<prev::core::memory::buffer::UBOPool<UniformsSkyCS> >(*allocator);
    m_uniformsPoolSkyCS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

    // compute sky post process
    m_conmputeSkyPostProcessShader = shaderFactory.CreateShaderFromFiles<prev_test::render::renderer::sky::shader::SkyPostProcessShader>(*device, { { VK_SHADER_STAGE_COMPUTE_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/sky_post_process_comp.spv") } });
    m_conmputeSkyPostProcessShader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Sky PostProcess Compute Shader created\n");

    m_computeSkyPostProcessPipeline = std::make_unique<prev_test::render::renderer::sky::pipeline::SkyPostProcessPipeline>(*device, *m_conmputeSkyPostProcessShader);
    m_computeSkyPostProcessPipeline->Init();

    LOGI("Sky PostProcess Compute Pipeline created\n");

    m_uniformsPoolSkyPorstProcessCS = std::make_unique<prev::core::memory::buffer::UBOPool<UniformsSkyPostProcessCS> >(*allocator);
    m_uniformsPoolSkyPorstProcessCS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

    // compositor
    m_shader = shaderFactory.CreateShaderFromFiles<prev_test::render::renderer::common::shader::ScreenSpaceShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/screen_space_vert.spv") }, { VK_SHADER_STAGE_FRAGMENT_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/screen_space_frag.spv") } });
    m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Sky Screen Space Shader created\n");

    m_pipeline = std::make_unique<prev_test::render::renderer::common::pipeline::ScreenSpacePipeline>(*device, *m_renderPass, *m_shader);
    m_pipeline->Init();

    LOGI("Sky Screen Space Pipeline created\n");
}

void SkyRenderer::BeforeRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
    const auto skyComponent = prev::scene::component::NodeComponentHelper::FindOne<SceneNodeFlags, ISkyComponent>(prev::common::FlagSet<SceneNodeFlags>{ SceneNodeFlags::SKY_RENDER_COMPONENT });
    const auto cloudsComponent = prev::scene::component::NodeComponentHelper::FindOne<SceneNodeFlags, ICloudsComponent>(prev::common::FlagSet<SceneNodeFlags>{ SceneNodeFlags::CLOUDS_COMPONENT });
    const auto mainLightComponent = prev::scene::component::NodeComponentHelper::FindOne<SceneNodeFlags, prev_test::component::light::ILightComponent>({ TAG_MAIN_LIGHT });
    const auto timeComponent = prev::scene::component::NodeComponentHelper::FindOne<SceneNodeFlags, prev_test::component::time::ITimeComponent>(prev::common::FlagSet<SceneNodeFlags>{ SceneNodeFlags::TIME_COMPONENT });

    auto device = prev::core::DeviceProvider::Instance().GetDevice();

    // generate clouds usgin compute queue
    auto computeQueue = prev::scene::ComputeProvider::Instance().GetQueue();
    auto computeAllocator = prev::scene::ComputeProvider::Instance().GetAllocator();

    UpdateImageBufferExtents(VkExtent2D{ renderContextUserData.extent.width, renderContextUserData.extent.height }, m_skyColorImageBuffer);
    UpdateImageBufferExtents(VkExtent2D{ renderContextUserData.extent.width, renderContextUserData.extent.height }, m_skyBloomImageBuffer);
    UpdateImageBufferExtents(VkExtent2D{ renderContextUserData.extent.width, renderContextUserData.extent.height }, m_skyAlphanessImageBuffer);
    UpdateImageBufferExtents(VkExtent2D{ renderContextUserData.extent.width, renderContextUserData.extent.height }, m_skyCloudDistanceImageBuffer);
    UpdateImageBufferExtents(VkExtent2D{ renderContextUserData.extent.width, renderContextUserData.extent.height }, m_skyPostProcessColorImageBuffer);

    auto commandPool = computeQueue->CreateCommandPool();
    auto commandBuffer = prev::util::VkUtils::CreateCommandBuffer(*device, commandPool);

    auto fence = prev::util::VkUtils::CreateFence(*device);

    VkCommandBufferBeginInfo commandBufferBeginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VKERRCHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

    // regular sky render
    auto uboCS = m_uniformsPoolSkyCS->GetNext();

    UniformsSkyCS uniformsCS{};
    uniformsCS.resolution = glm::vec4(renderContextUserData.extent.width, renderContextUserData.extent.height, 0.0f, 0.0f);
    uniformsCS.inverseProjectionMatrix = glm::inverse(renderContextUserData.projectionMatrix);
    uniformsCS.inverseViewMatrix = glm::inverse(renderContextUserData.viewMatrix);
    uniformsCS.lightColor = glm::vec4(mainLightComponent->GetColor(), 1.0f);
    uniformsCS.lightDirection = glm::vec4(-mainLightComponent->GetDirection(), 0.0f);
    uniformsCS.cameraPosition = glm::vec4(renderContextUserData.cameraPosition, 1.0f);
    uniformsCS.baseCloudColor = cloudsComponent->GetColor();
    uniformsCS.skyColorBottom = glm::vec4(skyComponent->GetBottomColor(), 1.0f);
    uniformsCS.skyColorTop = glm::vec4(skyComponent->GetTopColor(), 1.0f);
    uniformsCS.windDirection = glm::vec4(0.5f, 0.0f, 0.1f, 0.0f);
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

    m_conmputeSkyShader->Bind("uboCS", *uboCS);

    m_conmputeSkyShader->Bind("perlinNoiseTex", *cloudsComponent->GetPerlineNoise(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    m_conmputeSkyShader->Bind("weatherTex", *cloudsComponent->GetWeather(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    m_conmputeSkyShader->Bind("outFragColor", *m_skyColorImageBuffer, VK_IMAGE_LAYOUT_GENERAL);
    m_conmputeSkyShader->Bind("outBloom", *m_skyBloomImageBuffer, VK_IMAGE_LAYOUT_GENERAL);
    m_conmputeSkyShader->Bind("outAlphaness", *m_skyAlphanessImageBuffer, VK_IMAGE_LAYOUT_GENERAL);
    m_conmputeSkyShader->Bind("outCloudDistance", *m_skyCloudDistanceImageBuffer, VK_IMAGE_LAYOUT_GENERAL);

    const VkDescriptorSet descriptorSetCompute = m_conmputeSkyShader->UpdateNextDescriptorSet();

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, *m_computeSkyPipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_computeSkyPipeline->GetLayout(), 0, 1, &descriptorSetCompute, 0, 0);

    vkCmdDispatch(commandBuffer, 128, 128, 1);

    // barrier input - output image
    AddInterComputeImageBufferBarrier(m_skyColorImageBuffer->GetImage(), commandBuffer);
    AddInterComputeImageBufferBarrier(m_skyBloomImageBuffer->GetImage(), commandBuffer);
    AddInterComputeImageBufferBarrier(m_skyAlphanessImageBuffer->GetImage(), commandBuffer);
    AddInterComputeImageBufferBarrier(m_skyCloudDistanceImageBuffer->GetImage(), commandBuffer);

    // sky post process render
    auto uboPostCS = m_uniformsPoolSkyPorstProcessCS->GetNext();

    glm::vec4 lightPositionNdc = renderContextUserData.projectionMatrix * renderContextUserData.viewMatrix * glm::vec4(mainLightComponent->GetPosition(), 1.0f);
    lightPositionNdc = lightPositionNdc / lightPositionNdc.w;
    lightPositionNdc = lightPositionNdc * 0.5f + 0.5f;

    UniformsSkyPostProcessCS uniformsPostCS{};
    uniformsPostCS.resolution = glm::vec4(renderContextUserData.extent.width, renderContextUserData.extent.height, 0.0f, 0.0f);
    uniformsPostCS.lisghtPosition = lightPositionNdc;
    uniformsPostCS.enableGodRays = 1;
    uniformsPostCS.lightDotCameraFront = -glm::dot(glm::normalize(mainLightComponent->GetPosition() - renderContextUserData.cameraPosition), glm::normalize(prev::util::MathUtil::GetForwardVector(renderContextUserData.viewMatrix)));

    uboPostCS->Update(&uniformsPostCS);

    m_conmputeSkyPostProcessShader->Bind("uboCS", *uboPostCS);

    m_conmputeSkyPostProcessShader->Bind("skyTex", *m_skyColorImageBuffer, VK_IMAGE_LAYOUT_GENERAL);
    m_conmputeSkyPostProcessShader->Bind("bloomTex", *m_skyBloomImageBuffer, VK_IMAGE_LAYOUT_GENERAL);

    m_conmputeSkyPostProcessShader->Bind("outFragColor", *m_skyPostProcessColorImageBuffer, VK_IMAGE_LAYOUT_GENERAL);

    const VkDescriptorSet descriptorSetComputePost = m_conmputeSkyPostProcessShader->UpdateNextDescriptorSet();

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, *m_computeSkyPostProcessPipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_computeSkyPostProcessPipeline->GetLayout(), 0, 1, &descriptorSetComputePost, 0, 0);

    vkCmdDispatch(commandBuffer, 128, 128, 1);

    VKERRCHECK(vkEndCommandBuffer(commandBuffer));

    // Submit compute work
    vkResetFences(*device, 1, &fence);

    const VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    VkSubmitInfo computeSubmitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
    computeSubmitInfo.pWaitDstStageMask = &waitStageMask;
    computeSubmitInfo.commandBufferCount = 1;
    computeSubmitInfo.pCommandBuffers = &commandBuffer;
    VKERRCHECK(vkQueueSubmit(*computeQueue, 1, &computeSubmitInfo, fence));
    VKERRCHECK(vkWaitForFences(*device, 1, &fence, VK_TRUE, UINT64_MAX));

    vkDestroyFence(*device, fence, nullptr);
    vkDestroyCommandPool(*device, commandPool, nullptr);

    AddImageBufferBarrier(m_skyPostProcessColorImageBuffer->GetImage(), renderContext.commandBuffer);
}

void SkyRenderer::PreRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
    const VkRect2D scissor{ { 0, 0 }, renderContext.fullExtent };
    const VkViewport viewport{ 0, 0, static_cast<float>(renderContextUserData.extent.width), static_cast<float>(renderContextUserData.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void SkyRenderer::Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData)
{
    if (node->GetFlags().HasAll(prev::common::FlagSet<SceneNodeFlags>{ SceneNodeFlags::SKY_RENDER_COMPONENT })) {
        const auto skyComponent = prev::scene::component::ComponentRepository<ISkyComponent>::Instance().Get(node->GetId());

        m_shader->Bind("image", *m_skyPostProcessColorImageBuffer, VK_IMAGE_LAYOUT_GENERAL);

        const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
        const VkBuffer vertexBuffers[] = { *skyComponent->GetModel()->GetVertexBuffer() };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(renderContext.commandBuffer, *skyComponent->GetModel()->GetIndexBuffer(), 0, skyComponent->GetModel()->GetIndexBuffer()->GetIndexType());
        vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(renderContext.commandBuffer, skyComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
    }

    for (auto child : node->GetChildren()) {
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
    if (m_skyPostProcessColorImageBuffer) {
        m_skyPostProcessColorImageBuffer->Destroy();
        m_skyPostProcessColorImageBuffer = nullptr;
    }

    if (m_skyCloudDistanceImageBuffer) {
        m_skyCloudDistanceImageBuffer->Destroy();
        m_skyCloudDistanceImageBuffer = nullptr;
    }

    if (m_skyAlphanessImageBuffer) {
        m_skyAlphanessImageBuffer->Destroy();
        m_skyAlphanessImageBuffer = nullptr;
    }

    if (m_skyBloomImageBuffer) {
        m_skyBloomImageBuffer->Destroy();
        m_skyBloomImageBuffer = nullptr;
    }

    if (m_skyColorImageBuffer) {
        m_skyColorImageBuffer->Destroy();
        m_skyColorImageBuffer = nullptr;
    }

    m_shader->ShutDown();
    m_pipeline->ShutDown();

    m_conmputeSkyPostProcessShader->ShutDown();
    m_computeSkyPostProcessPipeline->ShutDown();

    m_conmputeSkyShader->ShutDown();
    m_computeSkyPipeline->ShutDown();
}

void SkyRenderer::UpdateImageBufferExtents(const VkExtent2D& extent, std::shared_ptr<prev::core::memory::image::IImageBuffer>& imageBuffer)
{
    auto computeAllocator = prev::scene::ComputeProvider::Instance().GetAllocator();
    if (imageBuffer == nullptr || imageBuffer->GetExtent().width != extent.width || imageBuffer->GetExtent().height != extent.height) {
        const prev::core::memory::image::ImageBufferCreateInfo bufferCreateInfo{ VkExtent2D{ extent.width, extent.height }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE };

        imageBuffer = std::make_unique<prev::core::memory::image::ImageStorageBuffer>(*computeAllocator);
        imageBuffer->Create(bufferCreateInfo);
    }
}

void SkyRenderer::AddInterComputeImageBufferBarrier(const VkImage image, VkCommandBuffer commandBuffer)
{
    // Barrier to ensure that texture is completely written and can be sampled in fragment shader
    VkImageMemoryBarrier fragColorBeforeShaderReadBufferBarrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
    fragColorBeforeShaderReadBufferBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    fragColorBeforeShaderReadBufferBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    fragColorBeforeShaderReadBufferBarrier.image = image;
    fragColorBeforeShaderReadBufferBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    fragColorBeforeShaderReadBufferBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
    fragColorBeforeShaderReadBufferBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    fragColorBeforeShaderReadBufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    fragColorBeforeShaderReadBufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &fragColorBeforeShaderReadBufferBarrier);
}

void SkyRenderer::AddImageBufferBarrier(const VkImage image, VkCommandBuffer commandBuffer)
{
    // Barrier to ensure that texture is completely written and can be sampled in fragment shader
    VkImageMemoryBarrier fragColorBeforeShaderReadBufferBarrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
    fragColorBeforeShaderReadBufferBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    fragColorBeforeShaderReadBufferBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    fragColorBeforeShaderReadBufferBarrier.image = image;
    fragColorBeforeShaderReadBufferBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    fragColorBeforeShaderReadBufferBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
    fragColorBeforeShaderReadBufferBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    fragColorBeforeShaderReadBufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    fragColorBeforeShaderReadBufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &fragColorBeforeShaderReadBufferBarrier);
}
} // namespace prev_test::render::renderer::sky
