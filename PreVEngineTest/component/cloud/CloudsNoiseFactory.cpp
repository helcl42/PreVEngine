#include "CloudsNoiseFactory.h"

#include "../../render/renderer/sky/pipeline/CloudsPerlinWorleyNoisePipeline.h"
#include "../../render/renderer/sky/shader/CloudsPerlinWorleyNoiseShader.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/render/buffer/image/ImageBufferFactory.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/util/VkUtils.h>

namespace prev_test::component::cloud {
CloudsNoiseImage CloudsNoiseFactory::CreatePerlinWorleyNoise(const uint32_t width, const uint32_t height, const uint32_t depth) const
{
    const auto noiseImageFormat{ VK_FORMAT_R8G8B8A8_UNORM };

    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };
    auto computeQueue{ device->GetQueue(prev::core::device::QueueType::COMPUTE) };

    prev::render::shader::ShaderFactory shaderFactory{};
    auto shader = shaderFactory.CreateShaderFromFiles<prev_test::render::renderer::sky::shader::CloudsPerlinWorleyNoiseShader>(*device, prev_test::render::renderer::sky::shader::CloudsPerlinWorleyNoiseShader::GetPaths());
    shader->Init();

    auto pipeline = std::make_unique<prev_test::render::renderer::sky::pipeline::CloudsPerlinWorleyNoisePipeline>(*device, *shader);
    pipeline->Init();

    auto commandPool = computeQueue->CreateCommandPool();
    auto commandBuffer = prev::util::vk::CreateCommandBuffer(*device, commandPool);

    auto fence = prev::util::vk::CreateFence(*device);

    const prev::render::buffer::image::ImageBufferCreateInfo imageBufferCreateInfo{ VkExtent3D{ width, height, depth }, VK_IMAGE_TYPE_3D, noiseImageFormat, VK_SAMPLE_COUNT_1_BIT, 0, true, VK_IMAGE_VIEW_TYPE_3D, 1 };
    auto noiseImageBuffer = prev::render::buffer::image::ImageBufferFactory{}.CreateStorage(imageBufferCreateInfo, *allocator);

    auto sampler = std::make_unique<prev::render::sampler::Sampler>(*device, static_cast<float>(noiseImageBuffer->GetMipLevels()), VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR);

    VKERRCHECK(vkQueueWaitIdle(*computeQueue));

    VkCommandBufferBeginInfo cmdBufBeginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    cmdBufBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VKERRCHECK(vkBeginCommandBuffer(commandBuffer, &cmdBufBeginInfo));

    shader->Bind("outVolumeTexture", noiseImageBuffer->GetImageView(), *sampler, VK_IMAGE_LAYOUT_GENERAL);
    const VkDescriptorSet descriptorSet = shader->UpdateNextDescriptorSet();

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, *pipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->GetLayout(), 0, 1, &descriptorSet, 0, 0);

    vkCmdDispatch(commandBuffer, 32, 32, 32);

    VKERRCHECK(vkEndCommandBuffer(commandBuffer));

    // Submit compute work
    vkResetFences(*device, 1, &fence);

    VkSubmitInfo computeSubmitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
    computeSubmitInfo.commandBufferCount = 1;
    computeSubmitInfo.pCommandBuffers = &commandBuffer;
    VKERRCHECK(vkQueueSubmit(*computeQueue, 1, &computeSubmitInfo, fence));
    VKERRCHECK(vkWaitForFences(*device, 1, &fence, VK_TRUE, UINT64_MAX));

    VKERRCHECK(vkQueueWaitIdle(*computeQueue));

    vkDestroyFence(*device, fence, nullptr);
    vkDestroyCommandPool(*device, commandPool, nullptr);

    allocator->TransitionImageLayout(noiseImageBuffer->GetImage(), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, noiseImageFormat, noiseImageBuffer->GetMipLevels());
    allocator->GenerateMipmaps(noiseImageBuffer->GetImage(), noiseImageBuffer->GetFormat(), noiseImageBuffer->GetExtent(), noiseImageBuffer->GetMipLevels(), noiseImageBuffer->GetLayerCount());

    pipeline->ShutDown();
    pipeline = nullptr;

    shader->ShutDown();
    shader = nullptr;

    return { std::move(noiseImageBuffer), std::move(sampler) };
}
} // namespace prev_test::component::cloud