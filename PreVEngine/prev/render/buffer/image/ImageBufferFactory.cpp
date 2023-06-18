#include "ImageBufferFactory.h"

#include "ColorImageBuffer.h"
#include "DepthImageBuffer.h"
#include "ImageBuffer.h"
#include "ImageStorageBuffer.h"

#include <stdexcept>

namespace prev::render::buffer::image {
std::unique_ptr<IImageBuffer> ImageBufferFactory::CreateColor(const ImageBufferCreateInfo& createInfo, prev::core::memory::Allocator& allocator) const
{
    auto buffer{ std::make_unique<ColorImageBuffer>(allocator, createInfo.extent, createInfo.imageType, createInfo.format, createInfo.flags, createInfo.samplesCount, createInfo.mipMap, createInfo.viewType, createInfo.layerCount) };
    return buffer;
}

std::unique_ptr<IImageBuffer> ImageBufferFactory::CreateDepth(const ImageBufferCreateInfo& createInfo, prev::core::memory::Allocator& allocator) const
{
    auto buffer{ std::make_unique<DepthImageBuffer>(allocator, createInfo.extent, createInfo.imageType, createInfo.format, createInfo.flags, createInfo.samplesCount, createInfo.mipMap, createInfo.viewType, createInfo.layerCount) };
    return buffer;
}

std::unique_ptr<IImageBuffer> ImageBufferFactory::CreateStorage(const ImageBufferCreateInfo& createInfo, prev::core::memory::Allocator& allocator) const
{
    auto buffer{ std::make_unique<ImageStorageBuffer>(allocator, createInfo.extent, createInfo.imageType, createInfo.format, createInfo.flags, createInfo.samplesCount, createInfo.mipMap, createInfo.viewType, createInfo.layerCount) };
    return buffer;
}

std::unique_ptr<IImageBuffer> ImageBufferFactory::CreateFromData(const ImageBufferCreateInfo& createInfo, prev::core::memory::Allocator& allocator) const
{
    if (createInfo.layerData.empty()) {
        throw std::runtime_error("ImageBufferFactory can not create image buffer without a provided data.");
    }

    auto buffer{ std::make_unique<ImageBuffer>(allocator, createInfo.extent, createInfo.imageType, createInfo.format, createInfo.flags, createInfo.samplesCount, createInfo.mipMap, createInfo.viewType, createInfo.layerCount, createInfo.layerData) };
    return buffer;
}
} // namespace prev::render::buffer::image