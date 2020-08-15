#ifndef __IMAGE_BUFFER_CREATE_INFO_H__
#define __IMAGE_BUFFER_CREATE_INFO_H__

#include "../Allocator.h"

namespace prev {
struct ImageBufferCreateInfo {
    ImageBufferCreateInfo(const VkExtent2D& ext, const VkImageType imgType, const VkFormat fmt, const VkImageCreateFlags flgs = 0, const bool mipmap = false, const bool filtering = true, const VkImageViewType vwType = VK_IMAGE_VIEW_TYPE_2D, const uint32_t lrCount = 1, const VkSamplerAddressMode mode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, const uint8_t* data = nullptr);

    ImageBufferCreateInfo(const VkExtent2D& ext, const VkImageType imgType, const VkFormat fmt, const VkImageCreateFlags flgs, const bool mipmap, const bool filtering, const VkImageViewType vwType, const uint32_t lrCount, const VkSamplerAddressMode mode, const std::vector<const uint8_t*>& lrImageData);

    ImageBufferCreateInfo(const VkExtent3D& ext, const VkImageType imgType, const VkFormat fmt, const VkImageCreateFlags flgs = 0, const bool mipmap = false, const bool filtering = true, const VkImageViewType vwType = VK_IMAGE_VIEW_TYPE_3D, const uint32_t lrCount = 1, const VkSamplerAddressMode mode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, const uint8_t* data = nullptr);

    ImageBufferCreateInfo(const VkExtent3D& ext, const VkImageType imgType, const VkFormat fmt, const VkImageCreateFlags flgs, const bool mipmap, const bool filtering, const VkImageViewType vwType, const uint32_t lrCount, const VkSamplerAddressMode mode, const std::vector<const uint8_t*>& lrImageData);

    ~ImageBufferCreateInfo() = default;

    const VkExtent3D extent;

    const VkImageType imageType;

    const VkFormat format;

    const VkImageCreateFlags flags;

    const bool mipMap;

    const bool filteringEnabled;

    const VkImageViewType viewType;

    const uint32_t layerCount;

    const VkSamplerAddressMode addressMode;

    const std::vector<const uint8_t*> layerData;
};

} // namespace prev

#endif // !__IIMAGE_BUFFER_H__
