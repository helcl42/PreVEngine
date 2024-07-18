#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include "../CommandsExecutor.h"
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
    Allocator(instance::Instance& instance, device::Device& device, device::Queue& queue, const VkDeviceSize blockSize = 256);

    ~Allocator();

public:
    void CreateBuffer(const void* data, const uint64_t size, const VkBufferUsageFlags usage, const MemoryType memtype, VkBuffer& buffer, VmaAllocation& alloc, void** mapped = nullptr);

    void DestroyBuffer(VkBuffer buffer, VmaAllocation alloc);

    void CreateImage(const VkExtent3D& extent, const VkImageType imageType, const VkFormat format, const VkSampleCountFlagBits sampleCount, const uint32_t mipLevels, const uint32_t layerCount, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkImageCreateFlags flags, VkImage& outImage, VmaAllocation& outAlloc);

    void DestroyImage(VkImage image, VmaAllocation alloc);

public:
    void CopyBuffer(const VkBuffer srcBuffer, const VkDeviceSize size, VkBuffer dstBuffer);

    void CopyBufferToImage(const VkExtent3D& extent, const VkBuffer buffer, const uint32_t layerIndex, VkImage image);

    void CopyDataToImage(const VkExtent3D& extent, const VkFormat format, const std::vector<const uint8_t*>& layerData, const uint32_t layerCount, VkImage& image);

public:
    device::Device& GetDevice() const;

    device::Queue& GetQueue() const;

private:
    instance::Instance& m_instance;

    device::Device& m_device;

    device::Queue& m_queue;

    VmaAllocator m_allocator{};

    std::unique_ptr<CommandsExecutor> m_commandsExecutor{};
};
} // namespace prev::core::memory

#endif