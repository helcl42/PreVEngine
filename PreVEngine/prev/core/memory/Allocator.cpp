#include "Allocator.h"

#include "../Core.h"
#include "../Formats.h"

#include "../../common/Logger.h"
#include "../../util/VkUtils.h"

#define NOMINMAX
// #define VMA_RECORDING_ENABLED 0
// #define VMA_DEDICATED_ALLOCATION 0
// #define VMA_STATS_STRING_ENABLED 1
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_IMPLEMENTATION
#include <external/vk_mem_alloc.h>

namespace prev::core::memory {
Allocator::Allocator(prev::core::instance::Instance& instance, prev::core::device::Device& device, prev::core::device::Queue& queue, const VkDeviceSize blockSize)
    : m_instance(instance)
    , m_device(device)
    , m_queue(queue)
{
    VmaVulkanFunctions fn{};
    fn.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
    fn.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
    fn.vkAllocateMemory = vkAllocateMemory;
    fn.vkBindBufferMemory = vkBindBufferMemory;
    fn.vkBindImageMemory = vkBindImageMemory;
    fn.vkCmdCopyBuffer = vkCmdCopyBuffer;
    fn.vkCreateBuffer = vkCreateBuffer;
    fn.vkCreateImage = vkCreateImage;
    fn.vkDestroyBuffer = vkDestroyBuffer;
    fn.vkDestroyImage = vkDestroyImage;
    fn.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
    fn.vkFreeMemory = vkFreeMemory;
    fn.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
    fn.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
    fn.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
    fn.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
    fn.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
    fn.vkMapMemory = vkMapMemory;
    fn.vkUnmapMemory = vkUnmapMemory;
    // fn.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR;
    // fn.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR;

    VmaAllocatorCreateInfo allocatorInfo{};
    allocatorInfo.instance = m_instance;
    allocatorInfo.physicalDevice = *m_device.GetGPU();
    allocatorInfo.device = m_device;
    allocatorInfo.preferredLargeHeapBlockSize = blockSize;
    allocatorInfo.pVulkanFunctions = &fn;

    VKERRCHECK(vmaCreateAllocator(&allocatorInfo, &m_allocator));

    m_commandsExecutor = std::make_unique<CommandsExecutor>(device, queue);
}

Allocator::~Allocator()
{
    m_commandsExecutor = nullptr;

    vmaDestroyAllocator(m_allocator);
}

void Allocator::CreateBuffer(const void* data, const uint64_t size, const VkBufferUsageFlags usage, const MemoryType memoryType, VkBuffer& outBuffer, VmaAllocation& outAlloc, void** outMapped)
{
    if (memoryType != MemoryType::DEVICE_LOCAL) { // For CPU-visible memory, skip staging buffer
        VkBufferCreateInfo bufInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufInfo.size = size;
        bufInfo.usage = usage;
        bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocCreateInfo = {};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        VmaAllocationInfo allocInfo = {};
        VKERRCHECK(vmaCreateBuffer(m_allocator, &bufInfo, &allocCreateInfo, &outBuffer, &outAlloc, &allocInfo));

        if (data) {
            memcpy(allocInfo.pMappedData, data, size);
        } else {
            memset(allocInfo.pMappedData, 0, size);
        }

        if (outMapped) {
            *outMapped = allocInfo.pMappedData;
        }
    } else { // For GPU-only memory, copy via staging buffer.
        VkBufferCreateInfo stagingBufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        stagingBufferCreateInfo.size = size;
        stagingBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        stagingBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo stagingAllocCreateInfo = {};
        stagingAllocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        stagingAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        VkBuffer stageBuffer = VK_NULL_HANDLE;
        VmaAllocation stageBufferAlloc = VK_NULL_HANDLE;

        VmaAllocationInfo allocInfo = {};
        VKERRCHECK(vmaCreateBuffer(m_allocator, &stagingBufferCreateInfo, &stagingAllocCreateInfo, &stageBuffer, &stageBufferAlloc, &allocInfo));

        memcpy(allocInfo.pMappedData, data, size);

        VkBufferCreateInfo bufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufferCreateInfo.size = size;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage;

        VmaAllocationCreateInfo allocCreateInfo = {};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocCreateInfo.flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        VKERRCHECK(vmaCreateBuffer(m_allocator, &bufferCreateInfo, &allocCreateInfo, &outBuffer, &outAlloc, nullptr));

        CopyBuffer(stageBuffer, bufferCreateInfo.size, outBuffer);

        vmaDestroyBuffer(m_allocator, stageBuffer, stageBufferAlloc);
    }
}

void Allocator::DestroyBuffer(VkBuffer buffer, VmaAllocation alloc)
{
    vmaDestroyBuffer(m_allocator, buffer, alloc);
}

void Allocator::CreateImage(const VkExtent3D& extent, const VkImageType imageType, const VkFormat format, const VkSampleCountFlagBits sampleCount, const uint32_t mipLevels, const uint32_t layerCount, const VkImageTiling tiling, const VkImageUsageFlags usage, const MemoryType memoryType, const VkImageCreateFlags flags, VkImage& outImage, VmaAllocation& outAlloc, void** outMapped)
{
    VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    imageInfo.flags = flags;
    imageInfo.imageType = imageType;
    imageInfo.format = format;
    imageInfo.extent = extent;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = layerCount;
    imageInfo.samples = sampleCount;
    imageInfo.tiling = tiling;
    imageInfo.usage = usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    if (memoryType != MemoryType::DEVICE_LOCAL) {
        allocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
    } else {
        allocCreateInfo.flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    }

    VmaAllocationInfo allocInfo = {};
    vmaCreateImage(m_allocator, &imageInfo, &allocCreateInfo, &outImage, &outAlloc, &allocInfo);

    if (outMapped && memoryType != MemoryType::DEVICE_LOCAL) {
        *outMapped = allocInfo.pMappedData;
    }
}

void Allocator::DestroyImage(VkImage image, VmaAllocation alloc)
{
    vmaDestroyImage(m_allocator, image, alloc);
}

void Allocator::CopyBuffer(const VkBuffer srcBuffer, const VkDeviceSize size, VkBuffer dstBuffer)
{
    m_commandsExecutor->ExecuteImmediate([&](VkCommandBuffer commandBuffer) {
        prev::util::vk::CopyBuffer(commandBuffer, srcBuffer, size, dstBuffer);
    });
}

void Allocator::CopyBufferToImage(const VkExtent3D& extent, const VkBuffer buffer, const uint32_t layerIndex, VkImage image)
{
    m_commandsExecutor->ExecuteImmediate([&](VkCommandBuffer commandBuffer) {
        prev::util::vk::CopyBufferToImage(commandBuffer, extent, buffer, layerIndex, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, image);
    });
}

void Allocator::CopyDataToImage(const VkExtent3D& extent, const VkFormat format, const std::vector<const uint8_t*>& layerData, const uint32_t layerCount, VkImage& image)
{
    for (uint32_t layerIndex = 0; layerIndex < layerCount; ++layerIndex) {
        // Copy image data to staging buffer in CPU memory
        const uint32_t formatSize{ format::FormatSize(format) };
        const uint64_t size{ extent.width * extent.height * extent.depth * formatSize };

        VkBufferCreateInfo bufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocStagingMemoryCreateInfo = {};
        allocStagingMemoryCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocStagingMemoryCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        VmaAllocationInfo allocStagingBufferInfo = {};
        VkBuffer stageBuffer = VK_NULL_HANDLE;
        VmaAllocation stageBufferAlloc = VK_NULL_HANDLE;
        VKERRCHECK(vmaCreateBuffer(m_allocator, &bufferCreateInfo, &allocStagingMemoryCreateInfo, &stageBuffer, &stageBufferAlloc, &allocStagingBufferInfo));

        // Copy image data to staging memory
        memcpy(allocStagingBufferInfo.pMappedData, layerData[layerIndex], size);

        // Copy image from staging buffer to image
        CopyBufferToImage(extent, stageBuffer, layerIndex, image);

        vmaDestroyBuffer(m_allocator, stageBuffer, stageBufferAlloc);
    }
}

prev::core::device::Device& Allocator::GetDevice() const
{
    return m_device;
}

prev::core::device::Queue& Allocator::GetQueue() const
{
    return m_queue;
}

} // namespace prev::core::memory