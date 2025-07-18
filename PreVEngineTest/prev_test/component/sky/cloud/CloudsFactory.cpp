#include "CloudsFactory.h"

#include "../../../common/AssetManager.h"

#include <prev/core/CommandsExecutor.h>
#include <prev/render/buffer/ImageBufferBuilder.h>
#include <prev/render/buffer/UniformBuffer.h>
#include <prev/render/pipeline/PipelineBuilder.h>
#include <prev/render/sampler/SamplerBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/util/VkUtils.h>

namespace prev_test::component::sky::cloud {
CloudsFactory::CloudsFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator)
    : m_device{ device }
    , m_allocator{ allocator }
{
}

Clouds CloudsFactory::Create(const uint32_t width, const uint32_t height) const
{
    struct Uniforms {
        DEFAULT_ALIGNMENT glm::vec4 textureSize;
        DEFAULT_ALIGNMENT glm::vec4 seed;
        DEFAULT_ALIGNMENT float perlinAmplitude;
        float perlinFrequency;
        float perlinScale;
        int perlinOctaves;
    };

    const auto& computeQueue{ m_device.GetQueue(prev::core::device::QueueType::COMPUTE) };

    // clang-format off
    auto shader = prev::render::shader::ShaderBuilder{ m_device }
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
    auto pipeline = prev::render::pipeline::ComputePipelineBuilder{ m_device, *shader }
        .Build();
    // clang-format on

    auto uniformBuffer = std::make_unique<prev::render::buffer::UnifomBuffer<Uniforms>>(m_allocator, static_cast<uint32_t>(m_device.GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

    auto weatherImageBuffer = prev::render::buffer::ImageBufferBuilder{ m_allocator }
                                  .SetExtent({ width, height, 1 })
                                  .SetFormat(VK_FORMAT_R8G8B8A8_UNORM)
                                  .SetType(VK_IMAGE_TYPE_2D)
                                  .SetMipMapEnabled(true)
                                  .SetUsageFlags(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT)
                                  .SetLayout(VK_IMAGE_LAYOUT_GENERAL)
                                  .Build();

    auto sampler = prev::render::sampler::SamplerBuilder{ m_device }
                       .SetAddressMode(VK_SAMPLER_ADDRESS_MODE_REPEAT)
                       .Build();

    prev::core::CommandsExecutor commandsExecutor{ m_device, computeQueue };
    commandsExecutor.ExecuteImmediate([&](VkCommandBuffer commandBuffer) {
        Uniforms uniforms{};
        uniforms.textureSize = glm::vec4(width, height, 0, 0);
        uniforms.seed = glm::vec4(10, 20, 0, 0);
        uniforms.perlinAmplitude = 0.5f;
        uniforms.perlinFrequency = 0.8f;
        uniforms.perlinScale = 95.0f;
        uniforms.perlinOctaves = 4;

        uniformBuffer->Data(uniforms);

        shader->Bind("uboCS", *uniformBuffer);
        shader->Bind("outWeatherTexture", *weatherImageBuffer, *sampler, VK_IMAGE_LAYOUT_GENERAL);
        const VkDescriptorSet descriptorSet = shader->UpdateNextDescriptorSet();

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, *pipeline);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->GetLayout(), 0, 1, &descriptorSet, 0, 0);

        vkCmdDispatch(commandBuffer, prev::util::vk::GetComputeGroupSize(width, 16), prev::util::vk::GetComputeGroupSize(height, 16), 1);

        weatherImageBuffer->GenerateMipMaps(commandBuffer);
        weatherImageBuffer->UpdateLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, commandBuffer);
    });

    pipeline = nullptr;
    shader = nullptr;

    return { std::move(weatherImageBuffer) };
}
} // namespace prev_test::component::sky::cloud
