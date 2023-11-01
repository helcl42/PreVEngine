#include "CloudsFactory.h"

#include "../../render/renderer/sky/pipeline/CloudsPipeline.h"
#include "../../render/renderer/sky/shader/CloudsShader.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/render/buffer/UniformBuffer.h>
#include <prev/render/buffer/image/ImageBufferFactory.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/util/VkUtils.h>

namespace prev_test::component::cloud {
CloudsImage CloudsFactory::Create(const uint32_t width, const uint32_t height) const
{
    struct Uniforms {
        DEFAULT_ALIGNMENT glm::vec4 textureSize;
        DEFAULT_ALIGNMENT glm::vec4 seed;
        DEFAULT_ALIGNMENT float perlinAmplitude;
        float perlinFrequency;
        float perlinScale;
        int perlinOctaves;
    };

    const auto weatherImageFormat{ VK_FORMAT_R8G8B8A8_UNORM };

    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };
    auto computeQueue{ device->GetQueue(prev::core::device::QueueType::COMPUTE) };

    prev::render::shader::ShaderFactory shaderFactory{};
    auto shader = shaderFactory.CreateShaderFromFiles<prev_test::render::renderer::sky::shader::CloudsShader>(*device, prev_test::render::renderer::sky::shader::CloudsShader::GetPaths());
    shader->Init();

    auto pipeline = std::make_unique<prev_test::render::renderer::sky::pipeline::CloudsPipeline>(*device, *shader);
    pipeline->Init();

    auto uniformsPool = std::make_unique<prev::render::buffer::UBOPool<Uniforms>>(*allocator);
    uniformsPool->AdjustCapactity(3, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));

    auto commandPool = computeQueue->CreateCommandPool();
    auto commandBuffer = prev::util::vk::CreateCommandBuffer(*device, commandPool);

    auto fence = prev::util::vk::CreateFence(*device);

    const prev::render::buffer::image::ImageBufferCreateInfo bufferCreateInfo{ VkExtent2D{ width, height }, VK_IMAGE_TYPE_2D, weatherImageFormat, VK_SAMPLE_COUNT_1_BIT, 0, true, VK_IMAGE_VIEW_TYPE_2D, 1 };
    auto weatherImageBuffer = prev::render::buffer::image::ImageBufferFactory{}.CreateStorage(bufferCreateInfo, *allocator);
    auto sampler = std::make_unique<prev::render::sampler::Sampler>(*device, static_cast<float>(weatherImageBuffer->GetMipLevels()), VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR);

    VKERRCHECK(vkQueueWaitIdle(*computeQueue));

    VkCommandBufferBeginInfo cmdBufBeginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    cmdBufBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VKERRCHECK(vkBeginCommandBuffer(commandBuffer, &cmdBufBeginInfo));

    Uniforms uniforms{};
    uniforms.textureSize = glm::vec4(width, height, 0, 0);
    uniforms.seed = glm::vec4(10, 20, 0, 0);
    uniforms.perlinAmplitude = 0.5f;
    uniforms.perlinFrequency = 0.8f;
    uniforms.perlinScale = 100.0f;
    uniforms.perlinOctaves = 4;

    auto ubo = uniformsPool->GetNext();
    ubo->Update(&uniforms);

    shader->Bind("uboCS", *ubo);
    shader->Bind("outWeatherTexture", weatherImageBuffer->GetImageView(), *sampler, VK_IMAGE_LAYOUT_GENERAL);
    const VkDescriptorSet descriptorSet = shader->UpdateNextDescriptorSet();

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, *pipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->GetLayout(), 0, 1, &descriptorSet, 0, 0);

    vkCmdDispatch(commandBuffer, 128, 128, 1);

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

    allocator->TransitionImageLayout(weatherImageBuffer->GetImage(), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, weatherImageFormat, weatherImageBuffer->GetMipLevels());
    allocator->GenerateMipmaps(weatherImageBuffer->GetImage(), weatherImageBuffer->GetFormat(), weatherImageBuffer->GetExtent(), weatherImageBuffer->GetMipLevels(), weatherImageBuffer->GetLayerCount());

    pipeline->ShutDown();
    pipeline = nullptr;

    shader->ShutDown();
    shader = nullptr;

    return { std::move(weatherImageBuffer), std::move(sampler) };
}
} // namespace prev_test::component::cloud