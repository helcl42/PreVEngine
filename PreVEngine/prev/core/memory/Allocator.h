#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include "../Queue.h"
#include <external/vk_mem_alloc.h>

#include <vector>

namespace prev::core::memory {
class Allocator final {
public:
    Allocator(const Queue& queue, const VkDeviceSize blockSize = 256);

    ~Allocator();

public:
    void BeginCommandBuffer();

    void EndCommandBuffer();

public:
    void TransitionImageLayout(const VkImage image, const VkImageLayout oldLayout, const VkImageLayout newLayout, const uint32_t mipLevels = 1, const uint32_t layersCount = 1);

    void CreateBuffer(const void* data, uint64_t size, VkBufferUsageFlags usage, VmaMemoryUsage memtype, VkBuffer& buffer, VmaAllocation& alloc, void** mapped = 0);

    void DestroyBuffer(VkBuffer buffer, VmaAllocation alloc);

    void CopyBuffer(const VkBuffer srcBuffer, const VkDeviceSize size, VkBuffer dstBuffer);

    void CopyBufferToImage(const VkExtent3D& extent, const VkBuffer buffer, const uint32_t layerIndex, VkImage image);

    void CreateImage(const VkExtent3D& extent, const VkImageType imageType, const VkFormat format, const uint32_t mipLevels, const uint32_t layerCount, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkImageCreateFlags flags, VkImage& outImage, VmaAllocation& outAlloc);

    void CopyDataToImage(const VkExtent3D& extent, const VkFormat format, const uint32_t mipLevels, const std::vector<const uint8_t*> layerData, const uint32_t layerCount, VkImage& image);

    void CreateImageView(const VkImage image, const VkFormat format, const VkImageViewType viewType, const uint32_t mipLevels, const uint32_t layerCount, const VkImageAspectFlags aspectFlags, VkImageView& outImagaView);

    void DestroyImage(VkImage image, VkImageView view, VmaAllocation alloc);

    void GenerateMipmaps(const VkImage image, const VkFormat imageFormat, const VkExtent3D& extent, const uint32_t mipLevels, const uint32_t layersCount = 1);

public:
    VkPhysicalDevice GetPhysicalDevice() const;

    VkDevice GetDevice() const;

    VkQueue GetQueue() const;

private:
    VmaAllocator m_allocator;

    VkPhysicalDevice m_gpu;

    VkDevice m_device;

    VkQueue m_queue;

    VkCommandPool m_commandPool;

    VkCommandBuffer m_commandBuffer;
};
} // namespace prev::core::memory

#endif