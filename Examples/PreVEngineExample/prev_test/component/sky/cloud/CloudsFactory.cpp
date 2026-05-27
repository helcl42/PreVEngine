#include "CloudsFactory.h"

#include "../../../common/ShaderAssetManager.h"

#include <prev/core/CommandsExecutor.h>
#include <prev/render/buffer/BufferBuilder.h>
#include <prev/render/buffer/ImageBufferBuilder.h>
#include <prev/render/buffer/ImageBufferViewBuilder.h>
#include <prev/render/pipeline/ComputePipelineBuilder.h>
#include <prev/render/sampler/SamplerBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/util/MathUtils.h>

namespace prev_test::component::sky::cloud {
CloudsFactory::CloudsFactory(prev::core::device::Device& device)
    : m_device{ device }
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
            { GFX_SHADER_STAGE_COMPUTE, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "sky/clouds_comp") }
        })
        .AddBindGroupEntries({
            prev::render::shader::ShaderBuilder::BindGroupEntry::Buffer("uboCS", 0, GFX_SHADER_STAGE_COMPUTE),
            prev::render::shader::ShaderBuilder::BindGroupEntry::StorageTexture("outWeatherTexture", 1, GFX_SHADER_STAGE_COMPUTE, GFX_FORMAT_R8G8B8A8_UNORM)
        })
	    .SetBindGroupCapacity(1)
        .Build();
    // clang-format on

    // clang-format off
    auto pipeline = prev::render::pipeline::ComputePipelineBuilder{ m_device, *shader }
        .Build();
    // clang-format on

    auto uniformBuffer = prev::render::buffer::BufferBuilder{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                             .SetSize(sizeof(Uniforms))
                             .SetUsageFlags(GFX_BUFFER_USAGE_UNIFORM | GFX_BUFFER_USAGE_MAP_WRITE)
                             .SetMemoryProperties(GFX_MEMORY_PROPERTY_HOST_VISIBLE | GFX_MEMORY_PROPERTY_HOST_COHERENT)
                             .SetAlignment(static_cast<uint32_t>(m_device.GetGPU().GetLimits().minUniformBufferOffsetAlignment))
                             .Build();

    auto weatherImageBuffer = prev::render::buffer::ImageBufferBuilder{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                                  .SetExtent({ width, height, 1 })
                                  .SetFormat(GFX_FORMAT_R8G8B8A8_UNORM)
                                  .SetType(GFX_TEXTURE_TYPE_2D)
                                  .SetMipMapEnabled(true)
                                  .SetUsageFlags(GFX_TEXTURE_USAGE_TEXTURE_BINDING | GFX_TEXTURE_USAGE_COPY_SRC | GFX_TEXTURE_USAGE_COPY_DST | GFX_TEXTURE_USAGE_STORAGE_BINDING)
                                  .SetLayout(GFX_TEXTURE_LAYOUT_GENERAL)
                                  .Build();

    auto sampler = prev::render::sampler::SamplerBuilder{ m_device }
                       .SetAddressMode(GFX_ADDRESS_MODE_REPEAT)
                       .Build();

    auto weatherStorageView = prev::render::buffer::ImageBufferViewBuilder{ *weatherImageBuffer }
                                  .SetMipLevelCount(1)
                                  .Build();

    prev::core::CommandsExecutor commandsExecutor{ m_device, computeQueue };
    commandsExecutor.ExecuteImmediate([&](GfxCommandEncoder commandBuffer) {
        Uniforms uniforms{};
        uniforms.textureSize = glm::vec4(width, height, 0, 0);
        uniforms.seed = glm::vec4(10, 20, 0, 0);
        uniforms.perlinAmplitude = 0.5f;
        uniforms.perlinFrequency = 0.8f;
        uniforms.perlinScale = 95.0f;
        uniforms.perlinOctaves = 4;
        uniformBuffer->Write(uniforms);

        shader->Bind("uboCS", *uniformBuffer);
        shader->Bind("outWeatherTexture", *weatherStorageView);
        const GfxBindGroup bindGroup = shader->UpdateNextBindGroup();

        GfxComputePassEncoder computePassEncoder{};
        GfxComputePassBeginDescriptor computePassDesc{};
        gfxCommandEncoderBeginComputePass(commandBuffer, &computePassDesc, &computePassEncoder);
        gfxComputePassEncoderSetPipeline(computePassEncoder, *pipeline);
        gfxComputePassEncoderSetBindGroup(computePassEncoder, 0, bindGroup, nullptr, 0);
        gfxComputePassEncoderDispatch(computePassEncoder, prev::util::math::DispatchSize(width, 16), prev::util::math::DispatchSize(height, 16), 1);
        gfxComputePassEncoderEnd(computePassEncoder);

        weatherImageBuffer->GenerateMipMaps(commandBuffer);
        weatherImageBuffer->UpdateLayout(GFX_TEXTURE_LAYOUT_SHADER_READ_ONLY, commandBuffer);
    });

    pipeline.reset();
    shader.reset();

    return { std::move(weatherImageBuffer) };
}
} // namespace prev_test::component::sky::cloud
