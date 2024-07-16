#include "CloudsFactory.h"

#include "../../../common/AssetManager.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/render/buffer/ImageBufferBuilder.h>
#include <prev/render/buffer/UniformBuffer.h>
#include <prev/render/pipeline/PipelineBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/util/VkUtils.h>

namespace prev_test::component::sky::cloud {
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

    // clang-format off
    auto shader = prev::render::shader::ShaderBuilder{ *device }
        .AddShaderStagePaths({
            { VK_SHADER_STAGE_COMPUTE_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/sky/clouds_comp.spv") }
        })
        .AddDescriptorSets({
            { "uboCS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT },
            { "outWeatherTexture", 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT }
        })
	    .SetDescriptorPoolCapacity(1)
        .Build();
    // clang-format on

    // clang-format off
    auto pipeline = prev::render::pipeline::ComputePipelineBuilder{ *device, *shader }
        .Build();
    // clang-format on

    auto uniformsPool = std::make_unique<prev::render::buffer::UniformBufferRing<Uniforms>>(*allocator);
    uniformsPool->AdjustCapactity(1, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));

    auto commandPool = prev::util::vk::CreateCommandPool(*device, computeQueue->family);
    auto commandBuffer = prev::util::vk::CreateCommandBuffer(*device, commandPool);

    auto fence = prev::util::vk::CreateFence(*device);

    auto weatherImageBuffer = prev::render::buffer::ImageBufferBuilder{ *allocator }
                                  .SetExtent({ width, height, 1 })
                                  .SetFormat(VK_FORMAT_R8G8B8A8_UNORM)
                                  .SetType(VK_IMAGE_TYPE_2D)
                                  .SetMipMapEnabled(true)
                                  .SetUsageFlags(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT)
                                  .SetLayout(VK_IMAGE_LAYOUT_GENERAL)
                                  .Build();
    auto sampler = std::make_unique<prev::render::sampler::Sampler>(*device, static_cast<float>(weatherImageBuffer->GetMipLevels()), VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR);

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
    shader->Bind("outWeatherTexture", *weatherImageBuffer, *sampler, VK_IMAGE_LAYOUT_GENERAL);
    const VkDescriptorSet descriptorSet = shader->UpdateNextDescriptorSet();

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, *pipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->GetLayout(), 0, 1, &descriptorSet, 0, 0);

    vkCmdDispatch(commandBuffer, 128, 128, 1);

    weatherImageBuffer->GenerateMipMaps(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, commandBuffer);

    VKERRCHECK(vkEndCommandBuffer(commandBuffer));

    // Submit compute work
    vkResetFences(*device, 1, &fence);

    VkSubmitInfo computeSubmitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
    computeSubmitInfo.commandBufferCount = 1;
    computeSubmitInfo.pCommandBuffers = &commandBuffer;
    VKERRCHECK(computeQueue->Submit(1, &computeSubmitInfo, fence));
    VKERRCHECK(vkWaitForFences(*device, 1, &fence, VK_TRUE, UINT64_MAX));

    vkDestroyFence(*device, fence, nullptr);
    vkDestroyCommandPool(*device, commandPool, nullptr);

    pipeline = nullptr;
    shader = nullptr;

    return { std::move(weatherImageBuffer), std::move(sampler) };
}
} // namespace prev_test::component::sky::cloud