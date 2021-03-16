#include "CloudsNoiseFactory.h"

#include "../../render/renderer/sky/pipeline/CloudsPerlinWorleyNoisePipeline.h"
#include "../../render/renderer/sky/shader/CloudsPerlinWorleyNoiseShader.h"

#include <prev/core/DeviceProvider.h>
#include <prev/core/memory/image/ImageStorageBuffer.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/ComputeProvider.h>
#include <prev/util/VkUtils.h>

namespace prev_test::component::cloud {
std::unique_ptr<prev::core::memory::image::IImageBuffer> CloudsNoiseFactory::CreatePerlinWorleyNoise(const uint32_t width, const uint32_t height, const uint32_t depth) const
{
    const auto noiseImageFormat{ VK_FORMAT_R8G8B8A8_UNORM };

    auto device = prev::core::DeviceProvider::Instance().GetDevice();
    auto computeQueue = prev::scene::ComputeProvider::Instance().GetQueue();
    auto computeAllocator = prev::scene::ComputeProvider::Instance().GetAllocator();

    prev::render::shader::ShaderFactory shaderFactory{};
    auto shader = shaderFactory.CreateShaderFromFiles<prev_test::render::renderer::sky::shader::CloudsPerlinWorleyNoiseShader>(*device, prev_test::render::renderer::sky::shader::CloudsPerlinWorleyNoiseShader::GetPaths());

    auto pipeline = std::make_unique<prev_test::render::renderer::sky::pipeline::CloudsPerlinWorleyNoisePipeline>(*device, *shader);
    pipeline->Init();

    auto commandPool = computeQueue->CreateCommandPool();
    auto commandBuffer = prev::util::VkUtils::CreateCommandBuffer(*device, commandPool);

    auto fence = prev::util::VkUtils::CreateFence(*device);

    prev::core::memory::image::ImageBufferCreateInfo imageBufferCreateInfo{ VkExtent3D{ width, height, depth }, VK_IMAGE_TYPE_3D, noiseImageFormat, VK_SAMPLE_COUNT_1_BIT, 0, true, true, VK_IMAGE_VIEW_TYPE_3D, 1, VK_SAMPLER_ADDRESS_MODE_REPEAT };

    auto noiseImageBuffer = std::make_unique<prev::core::memory::image::ImageStorageBuffer>(*computeAllocator);
    noiseImageBuffer->Create(imageBufferCreateInfo);

    VKERRCHECK(vkQueueWaitIdle(*computeQueue));

    VkCommandBufferBeginInfo cmdBufBeginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    cmdBufBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VKERRCHECK(vkBeginCommandBuffer(commandBuffer, &cmdBufBeginInfo));

    shader->Bind("outVolumeTexture", *noiseImageBuffer, VK_IMAGE_LAYOUT_GENERAL);
    const VkDescriptorSet descriptorSet = shader->UpdateNextDescriptorSet();

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, *pipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->GetLayout(), 0, 1, &descriptorSet, 0, 0);

    vkCmdDispatch(commandBuffer, 32, 32, 32);

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

    VKERRCHECK(vkQueueWaitIdle(*computeQueue));

    vkDestroyFence(*device, fence, nullptr);
    vkDestroyCommandPool(*device, commandPool, nullptr);

    computeAllocator->TransitionImageLayout(noiseImageBuffer->GetImage(), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, noiseImageFormat, noiseImageBuffer->GetMipLevels());

    computeAllocator->GenerateMipmaps(noiseImageBuffer->GetImage(), noiseImageBuffer->GetFormat(), noiseImageBuffer->GetExtent(), noiseImageBuffer->GetMipLevels(), noiseImageBuffer->GetLayerCount());

    pipeline->ShutDown();

    shader->ShutDown();

    return noiseImageBuffer;
}
} // namespace prev_test::component::cloud