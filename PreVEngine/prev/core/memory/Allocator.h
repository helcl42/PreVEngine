#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include "../device/Device.h"
#include "../instance/Instance.h"

#include <external/vk_mem_alloc.h>

#include <vector>

namespace prev::core::memory {
enum class MemoryType {
    DEVICE_LOCAL = 0,
    HOST_MAPPED = 1
};

class Allocator final {
public:
    Allocator(prev::core::instance::Instance& instance, prev::core::device::Device& device, prev::core::device::Queue& queue, const VkDeviceSize blockSize = 256);

    ~Allocator();

public:
    void CreateBuffer(const void* data, const uint64_t size, const VkBufferUsageFlags usage, const MemoryType memtype, VkBuffer& buffer, VmaAllocation& alloc, void** mapped = 0);

    void DestroyBuffer(VkBuffer buffer, VmaAllocation alloc);

    void CreateImage(const VkExtent3D& extent, const VkImageType imageType, const VkFormat format, const VkSampleCountFlagBits sampleCount, const uint32_t mipLevels, const uint32_t layerCount, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkImageCreateFlags flags, VkImage& outImage, VmaAllocation& outAlloc);

    void DestroyImage(VkImage image, VmaAllocation alloc);

public:
    void CopyBuffer(const VkBuffer srcBuffer, const VkDeviceSize size, VkBuffer dstBuffer);

    void CopyBufferToImage(const VkExtent3D& extent, const VkBuffer buffer, const uint32_t layerIndex, VkImage image);

    void CopyDataToImage(const VkExtent3D& extent, const VkFormat format, const uint32_t mipLevels, const std::vector<const uint8_t*>& layerData, const uint32_t layerCount, VkImage& image);

    void GenerateMipmaps(const VkImage image, const VkFormat imageFormat, const VkExtent3D& extent, const uint32_t mipLevels, const uint32_t layersCount = 1, const VkImageLayout newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    void TransitionImageLayout(const VkImage image, const VkImageLayout oldLayout, const VkImageLayout newLayout, const VkFormat format, const uint32_t mipLevels = 1, const uint32_t layersCount = 1);

public:
    prev::core::device::Device& GetDevice() const;

    prev::core::device::Queue& GetQueue() const;

private:
    void BeginCommandBuffer();

    void EndCommandBuffer();

private:
    prev::core::instance::Instance& m_instance;

    prev::core::device::Device& m_device;

    prev::core::device::Queue& m_queue;

    VkCommandPool m_commandPool{};

    VkCommandBuffer m_commandBuffer{};

    VkFence m_fence{};

    VmaAllocator m_allocator{};
};
} // namespace prev::core::memory

#endif