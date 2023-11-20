#include "Allocator.h"

#include "../Core.h"
#include "../Formats.h"

#include "../../common/Logger.h"
#include "../../util/VkUtils.h"

#define NOMINMAX
// #define VMA_RECORDING_ENABLED   0
// #define VMA_DEDICATED_ALLOCATION   0
// #define VMA_STATS_STRING_ENABLED   1
#define VMA_STATIC_VULKAN_FUNCTIONS 0

#define VMA_IMPLEMENTATION

// TODO - remove this android specific branch
#ifdef ANDROID
#include <external/android/vk_mem_alloc.h>
#else
#include <external/vk_mem_alloc.h>
#endif

namespace prev::core::memory {
Allocator::Allocator(prev::core::instance::Instance& instance, prev::core::device::Device& device, prev::core::device::Queue& queue, const VkDeviceSize blockSize)
    : m_instance(instance)
    , m_device(device)
    , m_queue(queue)
{
    m_commandPool = m_queue.CreateCommandPool();
    m_commandBuffer = prev::util::vk::CreateCommandBuffer(m_device, m_commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    m_fence = prev::util::vk::CreateFence(m_device);

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
}

Allocator::~Allocator()
{
    if (m_allocator) {
        vmaDestroyAllocator(m_allocator);
    }

    if (m_fence) {
        vkDestroyFence(m_device, m_fence, VK_NULL_HANDLE);
    }

    if (m_commandBuffer) {
        vkFreeCommandBuffers(m_device, m_commandPool, 1, &m_commandBuffer);
    }

    if (m_commandPool) {
        vkDestroyCommandPool(m_device, m_commandPool, VK_NULL_HANDLE);
    }
}

void Allocator::CreateBuffer(const void* data, uint64_t size, VkBufferUsageFlags usage, VmaMemoryUsage memtype, VkBuffer& buffer, VmaAllocation& alloc, void** mapped)
{
    if (memtype != VMA_MEMORY_USAGE_GPU_ONLY) // For CPU-visible memory, skip staging buffer
    {
        VkBufferCreateInfo bufInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufInfo.size = size;
        bufInfo.usage = usage;
        bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocCreateInfo = {};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
        allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

        VmaAllocationInfo allocInfo = {};
        VKERRCHECK(vmaCreateBuffer(m_allocator, &bufInfo, &allocCreateInfo, &buffer, &alloc, &allocInfo));

        if (data) {
            memcpy(allocInfo.pMappedData, data, size);
        } else {
            memset(allocInfo.pMappedData, 0, size);
        }

        if (mapped) {
            *mapped = allocInfo.pMappedData;
        }
    } else { // For GPU-only memory, copy via staging buffer.

        // TODO: Also skip staging buffer on integrated gpus.

        VkBufferCreateInfo stagingBufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        stagingBufferCreateInfo.size = size;
        stagingBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        stagingBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo stagingAllocCreateInfo = {};
        stagingAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
        stagingAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

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
        allocCreateInfo.usage = memtype;
        allocCreateInfo.flags = 0;
        VKERRCHECK(vmaCreateBuffer(m_allocator, &bufferCreateInfo, &allocCreateInfo, &buffer, &alloc, nullptr));

        CopyBuffer(stageBuffer, bufferCreateInfo.size, buffer);

        vmaDestroyBuffer(m_allocator, stageBuffer, stageBufferAlloc);
    }
}

void Allocator::DestroyBuffer(VkBuffer buffer, VmaAllocation alloc)
{
    vmaDestroyBuffer(m_allocator, buffer, alloc);
}

void Allocator::CreateImage(const VkExtent3D& extent, const VkImageType imageType, const VkFormat format, const VkSampleCountFlagBits sampleCount, const uint32_t mipLevels, const uint32_t layerCount, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkImageCreateFlags flags, VkImage& outImage, VmaAllocation& outAlloc)
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

    VmaAllocationCreateInfo allocGpuOnlyCreateInfo = {};
    allocGpuOnlyCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocGpuOnlyCreateInfo.flags = 0;
    vmaCreateImage(m_allocator, &imageInfo, &allocGpuOnlyCreateInfo, &outImage, &outAlloc, nullptr);
}

void Allocator::DestroyImage(VkImage image, VmaAllocation alloc)
{
    vmaDestroyImage(m_allocator, image, alloc);
}

void Allocator::CopyBuffer(const VkBuffer srcBuffer, const VkDeviceSize size, VkBuffer dstBuffer)
{
    BeginCommandBuffer();

    prev::util::vk::CopyBuffer(m_commandBuffer, srcBuffer, size, dstBuffer);

    EndCommandBuffer();
}

void Allocator::CopyBufferToImage(const VkExtent3D& extent, const VkBuffer buffer, const uint32_t layerIndex, VkImage image)
{
    BeginCommandBuffer();

    prev::util::vk::CopyBufferToImage(m_commandBuffer, extent, buffer, layerIndex, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, image);

    EndCommandBuffer();
}

void Allocator::CopyDataToImage(const VkExtent3D& extent, const VkFormat format, const uint32_t mipLevels, const std::vector<const uint8_t*> layerData, const uint32_t layerCount, VkImage& image)
{
    for (uint32_t layerIndex = 0; layerIndex < layerCount; ++layerIndex) {
        // Copy image data to staging buffer in CPU memory
        uint32_t formatSize = FormatSize(format);
        uint64_t size = extent.width * extent.height * extent.depth * formatSize;

        VkBufferCreateInfo bufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocStagingMemoryCreateInfo = {};
        allocStagingMemoryCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
        allocStagingMemoryCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

        VmaAllocationInfo allocStagingBufferInfo = {};
        VkBuffer stageBuffer = VK_NULL_HANDLE;
        VmaAllocation stageBufferAlloc = VK_NULL_HANDLE;
        VKERRCHECK(vmaCreateBuffer(m_allocator, &bufferCreateInfo, &allocStagingMemoryCreateInfo, &stageBuffer, &stageBufferAlloc, &allocStagingBufferInfo));

        // copy image data to staging memory
        memcpy(allocStagingBufferInfo.pMappedData, layerData[layerIndex], size);

        //  Copy image from staging buffer to image
        TransitionImageLayout(image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, format, mipLevels, layerCount);

        CopyBufferToImage(extent, stageBuffer, layerIndex, image);

        vmaDestroyBuffer(m_allocator, stageBuffer, stageBufferAlloc);
    }
}

void Allocator::GenerateMipmaps(const VkImage image, VkFormat imageFormat, const VkExtent3D& extent, const uint32_t mipLevels, const uint32_t layersCount, const VkImageLayout newLayout)
{
    // Check if image format supports linear blitting
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(*m_device.GetGPU(), imageFormat, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        LOGE("Texture image format does not support linear blitting!");
        return;
    }

    BeginCommandBuffer();

    prev::util::vk::GenerateMipmaps(m_commandBuffer, image, imageFormat, extent, mipLevels, layersCount, newLayout);

    EndCommandBuffer();
}

void Allocator::TransitionImageLayout(const VkImage image, const VkImageLayout oldLayout, const VkImageLayout newLayout, const VkFormat format, const uint32_t mipLevels, const uint32_t layersCount)
{
    BeginCommandBuffer();

    prev::util::vk::TransitionImageLayout(m_commandBuffer, image, oldLayout, newLayout, format, mipLevels, layersCount);

    EndCommandBuffer();
}

prev::core::device::Device& Allocator::GetDevice() const
{
    return m_device;
}

prev::core::device::Queue& Allocator::GetQueue() const
{
    return m_queue;
}

void Allocator::BeginCommandBuffer()
{
    VkCommandBufferBeginInfo cmdBufBeginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    cmdBufBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VKERRCHECK(vkBeginCommandBuffer(m_commandBuffer, &cmdBufBeginInfo));
}

void Allocator::EndCommandBuffer()
{
    VKERRCHECK(vkEndCommandBuffer(m_commandBuffer));

    vkResetFences(m_device, 1, &m_fence);

    VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffer;
    VKERRCHECK(vkQueueSubmit(m_queue, 1, &submitInfo, m_fence));
    VKERRCHECK(vkWaitForFences(m_device, 1, &m_fence, VK_TRUE, UINT64_MAX));
}

} // namespace prev::core::memory