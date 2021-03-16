#include "CloudsFactory.h"

#include "../../render/renderer/sky/pipeline/CloudsPipeline.h"
#include "../../render/renderer/sky/shader/CloudsShader.h"

#include <prev/core/DeviceProvider.h>
#include <prev/core/memory/image/ImageStorageBuffer.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/ComputeProvider.h>
#include <prev/util/VkUtils.h>

namespace prev_test::component::cloud {
std::unique_ptr<prev::core::memory::image::IImageBuffer> CloudsFactory::Create(const uint32_t width, const uint32_t height) const
{
    struct Uniforms {
        alignas(16) glm::vec4 textureSize;
        alignas(16) glm::vec4 seed;
        alignas(16) float perlinAmplitude;
        float perlinFrequency;
        float perlinScale;
        int perlinOctaves;
    };

    const auto weatherImageFormat{ VK_FORMAT_R8G8B8A8_UNORM };

    auto device = prev::core::DeviceProvider::Instance().GetDevice();
    auto computeQueue = prev::scene::ComputeProvider::Instance().GetQueue();
    auto computeAllocator = prev::scene::ComputeProvider::Instance().GetAllocator();

    prev::render::shader::ShaderFactory shaderFactory{};
    auto shader = shaderFactory.CreateShaderFromFiles<prev_test::render::renderer::sky::shader::CloudsShader>(*device, prev_test::render::renderer::sky::shader::CloudsShader::GetPaths());

    auto pipeline = std::make_unique<prev_test::render::renderer::sky::pipeline::CloudsPipeline>(*device, *shader);
    pipeline->Init();

    auto uniformsPool = std::make_unique<prev::core::memory::buffer::UBOPool<Uniforms> >(*computeAllocator);
    uniformsPool->AdjustCapactity(3, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

    auto commandPool = computeQueue->CreateCommandPool();
    auto commandBuffer = prev::util::VkUtils::CreateCommandBuffer(*device, commandPool);

    auto fence = prev::util::VkUtils::CreateFence(*device);

    prev::core::memory::image::ImageBufferCreateInfo bufferCreateInfo{ VkExtent2D{ width, height }, VK_IMAGE_TYPE_2D, weatherImageFormat, VK_SAMPLE_COUNT_1_BIT, 0, false, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_REPEAT };
    auto weatherImageBuffer = std::make_unique<prev::core::memory::image::ImageStorageBuffer>(*computeAllocator);
    weatherImageBuffer->Create(bufferCreateInfo);

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
    shader->Bind("outWeatherTexture", *weatherImageBuffer, VK_IMAGE_LAYOUT_GENERAL);
    const VkDescriptorSet descriptorSet = shader->UpdateNextDescriptorSet();

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, *pipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->GetLayout(), 0, 1, &descriptorSet, 0, 0);

    vkCmdDispatch(commandBuffer, 128, 128, 1);

    VKERRCHECK(vkEndCommandBuffer(commandBuffer));

    // Submit compute work
    vkResetFences(*device, 1, &fence);

    const VkPipelineStageFlags waitStageMask{ VK_PIPELINE_STAGE_TRANSFER_BIT };
    VkSubmitInfo computeSubmitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
    computeSubmitInfo.pWaitDstStageMask = &waitStageMask;
    computeSubmitInfo.commandBufferCount = 1;
    computeSubmitInfo.pCommandBuffers = &commandBuffer;
    VKERRCHECK(vkQueueSubmit(*computeQueue, 1, &computeSubmitInfo, fence));
    VKERRCHECK(vkWaitForFences(*device, 1, &fence, VK_TRUE, UINT64_MAX));

    VKERRCHECK(vkQueueWaitIdle(*computeQueue));

    vkDestroyFence(*device, fence, nullptr);
    vkDestroyCommandPool(*device, commandPool, nullptr);

    computeAllocator->TransitionImageLayout(weatherImageBuffer->GetImage(), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, weatherImageFormat, weatherImageBuffer->GetMipLevels());

    pipeline->ShutDown();

    shader->ShutDown();

    return weatherImageBuffer;
}
} // namespace prev_test::component::cloud