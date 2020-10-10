#ifndef __CLOUDS_FACTORY_H__
#define __CLOUDS_FACTORY_H__

#include <prev/core/memory/image/IImageBuffer.h>

namespace prev_test::component::cloud {
class CloudsFactory final {
public:
    std::unique_ptr<prev::core::memory::image::IImageBuffer> Create(const uint32_t width, const uint32_t height) const;
};
} // namespace prev_test::component::cloud

#endif // !__CLOUDS_FACTORY_H__
