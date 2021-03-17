#include "Allocator.h"
#include "../../util/VkUtils.h"
#include "../Formats.h"
#include "../instance/Validation.h"

#define NOMINMAX
//#define VMA_RECORDING_ENABLED   0
//#define VMA_DEDICATED_ALLOCATION   0
//#define VMA_STATS_STRING_ENABLED   1
#define VMA_STATIC_VULKAN_FUNCTIONS 0

#define VMA_IMPLEMENTATION
#include <external/vk_mem_alloc.h>

namespace prev::core::memory {
Allocator::Allocator(const std::shared_ptr<prev::core::device::Device>& device, const prev::core::device::QueueType queueType, const VkDeviceSize blockSize)
    : m_device(device)
{
    m_queue = m_device->GetQueue(queueType);
    m_commandPool = m_queue->CreateCommandPool();
    m_commandBuffer = prev::util::VkUtils::CreateCommandBuffer(*m_device, m_commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = *m_device->GetGPU();
    allocatorInfo.device = *m_device;
    allocatorInfo.preferredLargeHeapBlockSize = blockSize;

    VmaVulkanFunctions fn;
    fn.vkAllocateMemory = (PFN_vkAllocateMemory)vkAllocateMemory;
    fn.vkBindBufferMemory = (PFN_vkBindBufferMemory)vkBindBufferMemory;
    fn.vkBindImageMemory = (PFN_vkBindImageMemory)vkBindImageMemory;
    fn.vkCmdCopyBuffer = (PFN_vkCmdCopyBuffer)vkCmdCopyBuffer;
    fn.vkCreateBuffer = (PFN_vkCreateBuffer)vkCreateBuffer;
    fn.vkCreateImage = (PFN_vkCreateImage)vkCreateImage;
    fn.vkDestroyBuffer = (PFN_vkDestroyBuffer)vkDestroyBuffer;
    fn.vkDestroyImage = (PFN_vkDestroyImage)vkDestroyImage;
    fn.vkFlushMappedMemoryRanges = (PFN_vkFlushMappedMemoryRanges)vkFlushMappedMemoryRanges;
    fn.vkFreeMemory = (PFN_vkFreeMemory)vkFreeMemory;
    fn.vkGetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)vkGetBufferMemoryRequirements;
    fn.vkGetImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements)vkGetImageMemoryRequirements;
    fn.vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)vkGetPhysicalDeviceMemoryProperties;
    fn.vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)vkGetPhysicalDeviceProperties;
    fn.vkInvalidateMappedMemoryRanges = (PFN_vkInvalidateMappedMemoryRanges)vkInvalidateMappedMemoryRanges;
    fn.vkMapMemory = (PFN_vkMapMemory)vkMapMemory;
    fn.vkUnmapMemory = (PFN_vkUnmapMemory)vkUnmapMemory;
    fn.vkGetBufferMemoryRequirements2KHR = 0; //(PFN_vkGetBufferMemoryRequirements2KHR)vkGetBufferMemoryRequirements2KHR;
    fn.vkGetImageMemoryRequirements2KHR = 0; //(PFN_vkGetImageMemoryRequirements2KHR)vkGetImageMemoryRequirements2KHR;
    allocatorInfo.pVulkanFunctions = &fn;

    VKERRCHECK(vmaCreateAllocator(&allocatorInfo, &m_allocator));
}

Allocator::~Allocator()
{
    if (m_commandBuffer) {
        vkFreeCommandBuffers(*m_device, m_commandPool, 1, &m_commandBuffer);
    }

    if (m_commandPool) {
        vkDestroyCommandPool(*m_device, m_commandPool, nullptr);
    }

    if (m_allocator) {
        vmaDestroyAllocator(m_allocator);
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

void Allocator::DestroyImage(VkImage image, VkImageView view, VmaAllocation alloc)
{
    if (view) {
        vkDestroyImageView(*m_device, view, nullptr);
    }

    vmaDestroyImage(m_allocator, image, alloc);
}

void Allocator::CreateImageView(const VkImage image, const VkFormat format, const VkImageViewType viewType, const uint32_t mipLevels, const uint32_t layerCount, const VkImageAspectFlags aspectFlags, VkImageView& outImagaView)
{
    VkImageViewCreateInfo imageViewCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
    imageViewCreateInfo.image = image;
    imageViewCreateInfo.viewType = viewType;
    imageViewCreateInfo.format = format;
    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = mipLevels;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = layerCount;

    VKERRCHECK(vkCreateImageView(*m_device, &imageViewCreateInfo, nullptr, &outImagaView));
}

void Allocator::CopyBuffer(const VkBuffer srcBuffer, const VkDeviceSize size, VkBuffer dstBuffer)
{
    BeginCommandBuffer();

    VkBufferCopy bufCopyRegion = {};
    bufCopyRegion.srcOffset = 0;
    bufCopyRegion.dstOffset = 0;
    bufCopyRegion.size = size;

    vkCmdCopyBuffer(m_commandBuffer, srcBuffer, dstBuffer, 1, &bufCopyRegion);

    EndCommandBuffer();
}

void Allocator::CopyBufferToImage(const VkExtent3D& extent, const VkBuffer buffer, const uint32_t layerIndex, VkImage image)
{
    BeginCommandBuffer();

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = layerIndex;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = extent;

    vkCmdCopyBufferToImage(m_commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    EndCommandBuffer();
}

void Allocator::CopyDataToImage(const VkExtent3D& extent, const VkFormat format, const uint32_t mipLevels, const std::vector<const uint8_t*> layerData, const uint32_t layerCount, VkImage& image)
{
    for (uint32_t layerIndex = 0; layerIndex < layerCount; layerIndex++) {
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

void Allocator::GenerateMipmaps(const VkImage image, VkFormat imageFormat, const VkExtent3D& extent, const uint32_t mipLevels, const uint32_t layersCount)
{
    // Check if image format supports linear blitting
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(*m_device->GetGPU(), imageFormat, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        LOGE("Texture image format does not support linear blitting!");
        exit(0);
    }

    BeginCommandBuffer();

    for (uint32_t layerIndex = 0; layerIndex < layersCount; layerIndex++) {
        VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = layerIndex;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = extent.width;
        int32_t mipHeight = extent.height;
        int32_t mipDepth = extent.depth;

        for (uint32_t i = 1; i < mipLevels; i++) {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(m_commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

            if (mipDepth > 1) {
                for (int32_t d = 0; d < mipDepth; d += 2) {
                    VkImageBlit blit = {};
                    blit.srcOffsets[0] = { 0, 0, d };
                    blit.srcOffsets[1] = { mipWidth, mipHeight, d + 2 };
                    blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    blit.srcSubresource.mipLevel = i - 1;
                    blit.srcSubresource.baseArrayLayer = layerIndex;
                    blit.srcSubresource.layerCount = 1;
                    blit.dstOffsets[0] = { 0, 0, d / 2 };
                    blit.dstOffsets[1] = { std::max(1, mipWidth / 2), std::max(1, mipHeight / 2), d / 2 + 1 };
                    blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    blit.dstSubresource.mipLevel = i;
                    blit.dstSubresource.baseArrayLayer = layerIndex;
                    blit.dstSubresource.layerCount = 1;

                    vkCmdBlitImage(m_commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);
                }
            } else {
                VkImageBlit blit = {};
                blit.srcOffsets[0] = { 0, 0, 0 };
                blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
                blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.srcSubresource.mipLevel = i - 1;
                blit.srcSubresource.baseArrayLayer = layerIndex;
                blit.srcSubresource.layerCount = 1;
                blit.dstOffsets[0] = { 0, 0, 0 };
                blit.dstOffsets[1] = { std::max(1, mipWidth / 2), std::max(1, mipHeight / 2), 1 };
                blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.dstSubresource.mipLevel = i;
                blit.dstSubresource.baseArrayLayer = layerIndex;
                blit.dstSubresource.layerCount = 1;

                vkCmdBlitImage(m_commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);
            }

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(m_commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

            mipWidth = std::max(1, mipWidth / 2);
            mipHeight = std::max(1, mipHeight / 2);
            mipDepth = std::max(1, mipDepth / 2);
        }

        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(m_commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    EndCommandBuffer();
}

void Allocator::TransitionImageLayout(const VkImage image, const VkImageLayout oldLayout, const VkImageLayout newLayout, const VkFormat format, const uint32_t mipLevels, const uint32_t layersCount)
{
    BeginCommandBuffer();

    for (uint32_t layerIndex = 0; layerIndex < layersCount; layerIndex++) {
        VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = mipLevels;
        barrier.subresourceRange.baseArrayLayer = layerIndex;
        barrier.subresourceRange.layerCount = 1;

#if defined(__ANDROID__)
        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
#else
        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL || newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) {
#endif
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            if (HasStencilComponent(format)) {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        } else {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        VkPipelineStageFlags srcStage;
        VkPipelineStageFlags dstStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
#if defined(__ANDROID__)
#else
        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
#endif
        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_GENERAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
            srcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            dstStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_GENERAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            srcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_GENERAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            srcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else {
            LOGE("Unsupported layout transition\n");
        }

        vkCmdPipelineBarrier(m_commandBuffer, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    EndCommandBuffer();
}

std::shared_ptr<prev::core::device::Device> Allocator::GetDevice() const
{
    return m_device;
}

std::shared_ptr<prev::core::device::Queue> Allocator::GetQueue() const
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

    VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffer;
    VKERRCHECK(vkQueueSubmit(*m_queue, 1, &submitInfo, VK_NULL_HANDLE));

    VKERRCHECK(vkQueueWaitIdle(*m_queue));
}

} // namespace prev::core::memory