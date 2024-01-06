#ifndef __CLOUDS_NOISE_FACTORY_H__
#define __CLOUDS_NOISE_FACTORY_H__

#include <prev/render/buffer/image/IImageBuffer.h>
#include <prev/render/sampler/Sampler.h>

#include <memory>

namespace prev_test::component::sky::cloud {
struct CloudsNoiseImage {
    std::unique_ptr<prev::render::buffer::image::IImageBuffer> imageBuffer{};
    std::unique_ptr<prev::render::sampler::Sampler> imageSampler{};
};

class CloudsNoiseFactory final {
public:
    CloudsNoiseImage CreatePerlinWorleyNoise(const uint32_t width, const uint32_t height, const uint32_t depth) const;
};
} // namespace prev_test::component::sky::cloud

#endif // !__CLOUDS_NOISE_FACTORY_H__
