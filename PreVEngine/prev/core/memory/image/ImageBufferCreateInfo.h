#ifndef __IMAGE_BUFFER_CREATE_INFO_H__
#define __IMAGE_BUFFER_CREATE_INFO_H__

#include "../Allocator.h"

namespace prev::core::memory::image {
struct ImageBufferCreateInfo {
    ImageBufferCreateInfo(const VkExtent2D& ext, const VkImageType imgType, const VkFormat fmt, const VkSampleCountFlagBits samplesCnt = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT, const VkImageCreateFlags flgs = 0, const bool mipmap = false, const VkImageViewType vwType = VK_IMAGE_VIEW_TYPE_2D, const uint32_t lrCount = 1, const uint8_t* data = nullptr);

    ImageBufferCreateInfo(const VkExtent2D& ext, const VkImageType imgType, const VkFormat fmt, const VkSampleCountFlagBits samplesCnt, const VkImageCreateFlags flgs, const bool mipmap, const VkImageViewType vwType, const uint32_t lrCount, const std::vector<const uint8_t*>& lrImageData);

    ImageBufferCreateInfo(const VkExtent3D& ext, const VkImageType imgType, const VkFormat fmt, const VkSampleCountFlagBits samplesCnt = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT, const VkImageCreateFlags flgs = 0, const bool mipmap = false, const VkImageViewType vwType = VK_IMAGE_VIEW_TYPE_3D, const uint32_t lrCount = 1, const uint8_t* data = nullptr);

    ImageBufferCreateInfo(const VkExtent3D& ext, const VkImageType imgType, const VkFormat fmt, const VkSampleCountFlagBits samplesCnt, const VkImageCreateFlags flgs, const bool mipmap, const VkImageViewType vwType, const uint32_t lrCount, const std::vector<const uint8_t*>& lrImageData);

    ~ImageBufferCreateInfo() = default;

    const VkExtent3D extent;

    const VkImageType imageType;

    const VkFormat format;

    const VkSampleCountFlagBits samplesCount;

    const VkImageCreateFlags flags;

    const bool mipMap;

    const VkImageViewType viewType;

    const uint32_t layerCount;

    const std::vector<const uint8_t*> layerData;
};

} // namespace prev::core::memory::image

#endif // !__IIMAGE_BUFFER_H__
