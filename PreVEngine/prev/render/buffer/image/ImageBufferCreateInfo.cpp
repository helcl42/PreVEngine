#include "ImageBufferCreateInfo.h"

namespace prev::render::buffer::image {
ImageBufferCreateInfo::ImageBufferCreateInfo(const VkExtent2D& ext, const VkImageType imgType, const VkFormat fmt, const VkSampleCountFlagBits samplesCnt, const VkImageCreateFlags flgs, const bool mipmap, const VkImageViewType vwType, const uint32_t lrCount, const uint8_t* data)
    : ImageBufferCreateInfo(VkExtent3D{ ext.width, ext.height, 1 }, imgType, fmt, samplesCnt, flgs, mipmap, vwType, lrCount, std::vector<const uint8_t*>{ data })
{
}

ImageBufferCreateInfo::ImageBufferCreateInfo(const VkExtent2D& ext, const VkImageType imgType, const VkFormat fmt, const VkSampleCountFlagBits samplesCnt, const VkImageCreateFlags flgs, const bool mipmap, const VkImageViewType vwType, const uint32_t lrCount, const std::vector<const uint8_t*>& lrImageData)
    : ImageBufferCreateInfo(VkExtent3D{ ext.width, ext.height, 1 }, imgType, fmt, samplesCnt, flgs, mipmap, vwType, lrCount, lrImageData)
{
}

ImageBufferCreateInfo::ImageBufferCreateInfo(const VkExtent3D& ext, const VkImageType imgType, const VkFormat fmt, const VkSampleCountFlagBits samplesCnt, const VkImageCreateFlags flgs, const bool mipmap, const VkImageViewType vwType, const uint32_t lrCount, const uint8_t* data)
    : ImageBufferCreateInfo(ext, imgType, fmt, samplesCnt, flgs, mipmap, vwType, lrCount, std::vector<const uint8_t*>{ data })
{
}

ImageBufferCreateInfo::ImageBufferCreateInfo(const VkExtent3D& ext, const VkImageType imgType, const VkFormat fmt, const VkSampleCountFlagBits samplesCnt, const VkImageCreateFlags flgs, const bool mipmap, const VkImageViewType vwType, const uint32_t lrCount, const std::vector<const uint8_t*>& lrImageData)
    : extent(ext)
    , imageType(imgType)
    , format(fmt)
    , samplesCount(samplesCnt)
    , flags(flgs)
    , mipMap(mipmap)
    , viewType(vwType)
    , layerCount(lrCount)
    , layerData(lrImageData)
{
}
} // namespace prev::render::buffer::image