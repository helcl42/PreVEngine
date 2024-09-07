#include "CloudsNoiseFactory.h"

#include "../../../common/AssetManager.h"

#include <prev/core/CommandsExecutor.h>
#include <prev/render/buffer/ImageBufferBuilder.h>
#include <prev/render/pipeline/PipelineBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/util/VkUtils.h>

namespace prev_test::component::sky::cloud {
CloudsNoiseFactory::CloudsNoiseFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator)
    : m_device{ device }
    , m_allocator{ allocator }
{
}

CloudsNoiseImage CloudsNoiseFactory::CreatePerlinWorleyNoise(const uint32_t width, const uint32_t height, const uint32_t depth) const
{
    const auto noiseImageFormat{ VK_FORMAT_R8G8B8A8_UNORM };

    auto computeQueue{ m_device.GetQueue(prev::core::device::QueueType::COMPUTE) };

    // clang-format off
    auto shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { VK_SHADER_STAGE_COMPUTE_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/sky/clouds_perlin_worley_noise_3d_comp.spv") }
        })
        .AddDescriptorSets({
            { "outVolumeTexture", 0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT }
        })
	    .SetDescriptorPoolCapacity(1)
        .Build();
    // clang-format on

    // clang-format off
    auto pipeline = prev::render::pipeline::ComputePipelineBuilder{ m_device, *shader }
        .Build();
    // clang-format on

    auto noiseImageBuffer = prev::render::buffer::ImageBufferBuilder{ m_allocator }
                                .SetExtent({ width, height, depth })
                                .SetFormat(noiseImageFormat)
                                .SetType(VK_IMAGE_TYPE_3D)
                                .SetMipMapEnabled(true)
                                .SetUsageFlags(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT)
                                .SetLayout(VK_IMAGE_LAYOUT_GENERAL)
                                .Build();
    auto sampler = std::make_unique<prev::render::sampler::Sampler>(m_device, static_cast<float>(noiseImageBuffer->GetMipLevels()), VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR);

    prev::core::CommandsExecutor commandsExecutor{ m_device, *computeQueue };
    commandsExecutor.ExecuteImmediate([&](VkCommandBuffer commandBuffer) {
        shader->Bind("outVolumeTexture", *noiseImageBuffer, *sampler, VK_IMAGE_LAYOUT_GENERAL);
        const VkDescriptorSet descriptorSet = shader->UpdateNextDescriptorSet();

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, *pipeline);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->GetLayout(), 0, 1, &descriptorSet, 0, 0);

        vkCmdDispatch(commandBuffer, 32, 32, 32);

        noiseImageBuffer->GenerateMipMaps(commandBuffer);
        noiseImageBuffer->UpdateLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, commandBuffer);
    });

    pipeline = nullptr;
    shader = nullptr;

    return { std::move(noiseImageBuffer), std::move(sampler) };
}
} // namespace prev_test::component::sky::cloud
