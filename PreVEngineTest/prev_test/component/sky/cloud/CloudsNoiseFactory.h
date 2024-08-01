#ifndef __CLOUDS_NOISE_FACTORY_H__
#define __CLOUDS_NOISE_FACTORY_H__

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/render/buffer/ImageBuffer.h>
#include <prev/render/sampler/Sampler.h>

#include <memory>

namespace prev_test::component::sky::cloud {
struct CloudsNoiseImage {
    std::unique_ptr<prev::render::buffer::ImageBuffer> imageBuffer{};
    std::unique_ptr<prev::render::sampler::Sampler> imageSampler{};
};

class CloudsNoiseFactory final {
public:
    CloudsNoiseFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator);

    ~CloudsNoiseFactory() = default;

public:
    CloudsNoiseImage CreatePerlinWorleyNoise(const uint32_t width, const uint32_t height, const uint32_t depth) const;

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;
};
} // namespace prev_test::component::sky::cloud

#endif // !__CLOUDS_NOISE_FACTORY_H__
