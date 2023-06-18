#ifndef __IMAGE_BUFFER_FACTORY_H__
#define __IMAGE_BUFFER_FACTORY_H__

#include "IImageBuffer.h"
#include "ImageBufferCreateInfo.h"

#include <prev/core/memory/Allocator.h>

#include <memory>

namespace prev::render::buffer::image {
class ImageBufferFactory final {
public:
    std::unique_ptr<IImageBuffer> CreateColor(const ImageBufferCreateInfo& createInfo, prev::core::memory::Allocator& allocator) const;

    std::unique_ptr<IImageBuffer> CreateDepth(const ImageBufferCreateInfo& createInfo, prev::core::memory::Allocator& allocator) const;

    std::unique_ptr<IImageBuffer> CreateStorage(const ImageBufferCreateInfo& createInfo, prev::core::memory::Allocator& allocator) const;

    std::unique_ptr<IImageBuffer> CreateFromData(const ImageBufferCreateInfo& createInfo, prev::core::memory::Allocator& allocator) const;
};
} // namespace prev::render::buffer::image

#endif // !__IMAGE_BUFFER_FACTORY_H__
