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
    Allocator(const instance::Instance& instance, const device::Device& device, const device::Queue& queue, const VkDeviceSize blockSize = 256);

    ~Allocator();

public:
    void CreateBuffer(const void* data, const uint64_t size, const VkBufferUsageFlags usage, const MemoryType memoryType, VkBuffer& outBuffer, VmaAllocation& outAlloc, void** outMapped = nullptr);

    void DestroyBuffer(VkBuffer buffer, VmaAllocation alloc);

    void CreateImage(const VkExtent3D& extent, const VkImageType imageType, const VkFormat format, const VkSampleCountFlagBits sampleCount, const uint32_t mipLevels, const uint32_t layerCount, const VkImageTiling tiling, const VkImageUsageFlags usage, const MemoryType memoryType, const VkImageCreateFlags flags, VkImage& outImage, VmaAllocation& outAlloc, void** outMapped = nullptr);

    void DestroyImage(VkImage image, VmaAllocation alloc);

public:
    void CopyBuffer(const VkBuffer srcBuffer, const VkDeviceSize size, VkBuffer dstBuffer);

    void CopyBufferToImage(const VkExtent3D& extent, const VkBuffer buffer, const uint32_t layerIndex, VkImage image);

    void CopyDataToImage(const VkExtent3D& extent, const VkFormat format, const std::vector<const uint8_t*>& layerData, const uint32_t layerCount, VkImage& image);

public:
    const device::Device& GetDevice() const;

    const device::Queue& GetQueue() const;

private:
    const instance::Instance& m_instance;

    const device::Device& m_device;

    const device::Queue& m_queue;

    VmaAllocator m_allocator{};

    std::unique_ptr<CommandsExecutor> m_commandsExecutor{};
};
} // namespace prev::core::memory

#endif