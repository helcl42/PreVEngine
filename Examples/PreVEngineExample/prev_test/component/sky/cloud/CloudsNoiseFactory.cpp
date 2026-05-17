#include "CloudsNoiseFactory.h"

#include "../../../common/ShaderAssetManager.h"

#include <prev/core/CommandsExecutor.h>
#include <prev/render/buffer/ImageBufferBuilder.h>
#include <prev/render/buffer/ImageBufferViewBuilder.h>
#include <prev/render/pipeline/ComputePipelineBuilder.h>
#include <prev/render/sampler/SamplerBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>

namespace prev_test::component::sky::cloud {
CloudsNoiseFactory::CloudsNoiseFactory(prev::core::device::Device& device)
    : m_device{ device }
{
}

CloudsNoise CloudsNoiseFactory::CreatePerlinWorleyNoise(const uint32_t width, const uint32_t height, const uint32_t depth) const
{
    const auto noiseImageFormat{ GFX_FORMAT_R8G8B8A8_UNORM };

    const auto& computeQueue{ m_device.GetQueue(prev::core::device::QueueType::COMPUTE) };

    // clang-format off
    auto shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { GFX_SHADER_STAGE_COMPUTE, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "sky/clouds_perlin_worley_noise_3d_comp") }
        })
        .AddBindGroupEntries({
            prev::render::shader::ShaderBuilder::BindGroupEntry::StorageTexture("outVolumeTexture", 0, GFX_SHADER_STAGE_COMPUTE, noiseImageFormat, GFX_TEXTURE_VIEW_TYPE_3D)
        })
	    .SetBindGroupCapacity(1)
        .Build();
    // clang-format on

    // clang-format off
    auto pipeline = prev::render::pipeline::ComputePipelineBuilder{ m_device, *shader }
        .Build();
    // clang-format on

    auto noiseImageBuffer = prev::render::buffer::ImageBufferBuilder{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                                .SetExtent({ width, height, depth })
                                .SetFormat(noiseImageFormat)
                                .SetType(GFX_TEXTURE_TYPE_3D)
                                .SetMipMapEnabled(true)
                                .SetUsageFlags(GFX_TEXTURE_USAGE_TEXTURE_BINDING | GFX_TEXTURE_USAGE_COPY_SRC | GFX_TEXTURE_USAGE_COPY_DST | GFX_TEXTURE_USAGE_STORAGE_BINDING)
                                .SetLayout(GFX_TEXTURE_LAYOUT_GENERAL)
                                .Build();

    auto sampler = prev::render::sampler::SamplerBuilder{ m_device }
                       .SetAddressMode(GFX_ADDRESS_MODE_REPEAT)
                       .Build();

    auto noiseStorageView = prev::render::buffer::ImageBufferViewBuilder{ *noiseImageBuffer }
                                .SetMipLevelCount(1)
                                .Build();

    prev::core::CommandsExecutor commandsExecutor{ m_device, computeQueue };
    commandsExecutor.ExecuteImmediate([&](GfxCommandEncoder commandBuffer) {
        shader->Bind("outVolumeTexture", *noiseStorageView);
        const GfxBindGroup bindGroup = shader->UpdateNextBindGroup();

        GfxComputePassEncoder computePassEncoder{};
        GfxComputePassBeginDescriptor computePassDesc{};
        gfxCommandEncoderBeginComputePass(commandBuffer, &computePassDesc, &computePassEncoder);
        gfxComputePassEncoderSetPipeline(computePassEncoder, *pipeline);
        gfxComputePassEncoderSetBindGroup(computePassEncoder, 0, bindGroup, nullptr, 0);
        gfxComputePassEncoderDispatch(computePassEncoder, 32, 32, 32);
        gfxComputePassEncoderEnd(computePassEncoder);

        noiseImageBuffer->GenerateMipMaps(commandBuffer);
        noiseImageBuffer->UpdateLayout(GFX_TEXTURE_LAYOUT_SHADER_READ_ONLY, commandBuffer);
    });

    pipeline = nullptr;
    shader = nullptr;

    return { std::move(noiseImageBuffer) };
}
} // namespace prev_test::component::sky::cloud
