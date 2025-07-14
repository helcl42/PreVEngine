#ifndef __CLOUDS_FACTORY_H__
#define __CLOUDS_FACTORY_H__

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/render/buffer/ImageBuffer.h>

#include <memory>

namespace prev_test::component::sky::cloud {
struct Clouds {
    std::unique_ptr<prev::render::buffer::ImageBuffer> imageBuffer{};
};

class CloudsFactory final {
public:
    CloudsFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator);

    ~CloudsFactory() = default;

public:
    Clouds Create(const uint32_t width, const uint32_t height) const;

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;
};
} // namespace prev_test::component::sky::cloud

#endif // !__CLOUDS_FACTORY_H__
