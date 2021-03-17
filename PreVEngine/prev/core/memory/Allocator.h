#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include "../device/Device.h"

#include <external/vk_mem_alloc.h>

#include <vector>

namespace prev::core::memory {
class Allocator final {
public:
    Allocator(const std::shared_ptr<prev::core::device::Device>& device, const prev::core::device::QueueType queueType, const VkDeviceSize blockSize = 256);

    ~Allocator();

public:
    void CreateBuffer(const void* data, uint64_t size, VkBufferUsageFlags usage, VmaMemoryUsage memtype, VkBuffer& buffer, VmaAllocation& alloc, void** mapped = 0);

    void DestroyBuffer(VkBuffer buffer, VmaAllocation alloc);

    void CreateImage(const VkExtent3D& extent, const VkImageType imageType, const VkFormat format, const VkSampleCountFlagBits sampleCount, const uint32_t mipLevels, const uint32_t layerCount, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkImageCreateFlags flags, VkImage& outImage, VmaAllocation& outAlloc);

    void DestroyImage(VkImage image, VkImageView view, VmaAllocation alloc);

    void CreateImageView(const VkImage image, const VkFormat format, const VkImageViewType viewType, const uint32_t mipLevels, const uint32_t layerCount, const VkImageAspectFlags aspectFlags, VkImageView& outImagaView);

public:
    void CopyBuffer(const VkBuffer srcBuffer, const VkDeviceSize size, VkBuffer dstBuffer);

    void CopyBufferToImage(const VkExtent3D& extent, const VkBuffer buffer, const uint32_t layerIndex, VkImage image);

    void CopyDataToImage(const VkExtent3D& extent, const VkFormat format, const uint32_t mipLevels, const std::vector<const uint8_t*> layerData, const uint32_t layerCount, VkImage& image);

    void GenerateMipmaps(const VkImage image, const VkFormat imageFormat, const VkExtent3D& extent, const uint32_t mipLevels, const uint32_t layersCount = 1);

    void TransitionImageLayout(const VkImage image, const VkImageLayout oldLayout, const VkImageLayout newLayout, const VkFormat format, const uint32_t mipLevels = 1, const uint32_t layersCount = 1);

public:
    std::shared_ptr<prev::core::device::Device> GetDevice() const;

    std::shared_ptr<prev::core::device::Queue> GetQueue() const;

private:
    void BeginCommandBuffer();

    void EndCommandBuffer();

private:
    std::shared_ptr<prev::core::device::Device> m_device;

    VmaAllocator m_allocator;

    std::shared_ptr<prev::core::device::Queue> m_queue;

    VkCommandPool m_commandPool;

    VkCommandBuffer m_commandBuffer;
};
} // namespace prev::core::memory

#endif