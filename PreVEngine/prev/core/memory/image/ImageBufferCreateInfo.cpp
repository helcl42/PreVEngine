#include "ImageBufferCreateInfo.h"

namespace prev::core::memory::image {
ImageBufferCreateInfo::ImageBufferCreateInfo(const VkExtent2D& ext, const VkImageType imgType, const VkFormat fmt, const VkSampleCountFlagBits samplesCnt, const VkImageCreateFlags flgs, const bool mipmap, const bool filtering, const VkImageViewType vwType, const uint32_t lrCount, const VkSamplerAddressMode mode, const uint8_t* data)
    : ImageBufferCreateInfo(VkExtent3D{ ext.width, ext.height, 1 }, imgType, fmt, samplesCnt, flgs, mipmap, filtering, vwType, lrCount, mode, std::vector<const uint8_t*>{ data })
{
}

ImageBufferCreateInfo::ImageBufferCreateInfo(const VkExtent2D& ext, const VkImageType imgType, const VkFormat fmt, const VkSampleCountFlagBits samplesCnt, const VkImageCreateFlags flgs, const bool mipmap, const bool filtering, const VkImageViewType vwType, const uint32_t lrCount, const VkSamplerAddressMode mode, const std::vector<const uint8_t*>& lrImageData)
    : ImageBufferCreateInfo(VkExtent3D{ ext.width, ext.height, 1 }, imgType, fmt, samplesCnt, flgs, mipmap, filtering, vwType, lrCount, mode, lrImageData)
{
}

ImageBufferCreateInfo::ImageBufferCreateInfo(const VkExtent3D& ext, const VkImageType imgType, const VkFormat fmt, const VkSampleCountFlagBits samplesCnt, const VkImageCreateFlags flgs, const bool mipmap, const bool filtering, const VkImageViewType vwType, const uint32_t lrCount, const VkSamplerAddressMode mode, const uint8_t* data)
    : ImageBufferCreateInfo(ext, imgType, fmt, samplesCnt, flgs, mipmap, filtering, vwType, lrCount, mode, std::vector<const uint8_t*>{ data })
{
}

ImageBufferCreateInfo::ImageBufferCreateInfo(const VkExtent3D& ext, const VkImageType imgType, const VkFormat fmt, const VkSampleCountFlagBits samplesCnt, const VkImageCreateFlags flgs, const bool mipmap, const bool filtering, const VkImageViewType vwType, const uint32_t lrCount, const VkSamplerAddressMode mode, const std::vector<const uint8_t*>& lrImageData)
    : extent(ext)
    , imageType(imgType)
    , format(fmt)
    , samplesCount(samplesCnt)
    , flags(flgs)
    , mipMap(mipmap)
    , filteringEnabled(filtering)
    , viewType(vwType)
    , layerCount(lrCount)
    , addressMode(mode)
    , layerData(lrImageData)
{
}
} // namespace prev::core::memory::image