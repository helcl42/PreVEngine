#ifndef __CLOUDS_NOISE_FACTORY_H__
#define __CLOUDS_NOISE_FACTORY_H__

#include <prev/core/memory/image/IImageBuffer.h>

namespace prev_test::component::cloud {
class CloudsNoiseFactory final {
public:
    std::unique_ptr<prev::core::memory::image::IImageBuffer> CreatePerlinWorleyNoise(const uint32_t width, const uint32_t height, const uint32_t depth) const;
};
} // namespace prev_test::component::cloud

#endif // !__CLOUDS_NOISE_FACTORY_H__
