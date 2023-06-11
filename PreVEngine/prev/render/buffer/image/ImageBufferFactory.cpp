#include "ImageBufferFactory.h"

#include "ColorImageBuffer.h"
#include "DepthImageBuffer.h"
#include "ImageStorageBuffer.h"

namespace prev::render::buffer::image {
std::unique_ptr<IImageBuffer> ImageBufferFactory::CreateColor(const ImageBufferCreateInfo& createInfo, prev::core::memory::Allocator& allocator) const
{
    auto buffer{ std::make_unique<ColorImageBuffer>(allocator) };
    buffer->Create(createInfo);
    return buffer;
}

std::unique_ptr<IImageBuffer> ImageBufferFactory::CreateDepth(const ImageBufferCreateInfo& createInfo, prev::core::memory::Allocator& allocator) const
{
    auto buffer{ std::make_unique<DepthImageBuffer>(allocator) };
    buffer->Create(createInfo);
    return buffer;
}

std::unique_ptr<IImageBuffer> ImageBufferFactory::CreateStorage(const ImageBufferCreateInfo& createInfo, prev::core::memory::Allocator& allocator) const
{
    auto buffer{ std::make_unique<ImageStorageBuffer>(allocator) };
    buffer->Create(createInfo);
    return buffer;
}
} // namespace prev::render::buffer::image