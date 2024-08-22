#include "VkUtils.h"

#include "../common/Logger.h"
#include "../core/Formats.h"

#include <algorithm>
#include <limits>
#include <map>
#include <sstream>

namespace prev::util::vk {
uint32_t FindMemoryType(const VkPhysicalDevice gpu, const uint32_t typeFilter, const VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(gpu, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    assert(false && "failed to find suitable memory type!");
    return std::numeric_limits<uint32_t>::max();
}

void CreateImage(const VkPhysicalDevice gpu, const VkDevice device, const VkExtent2D& extent, const VkImageType imageType, const VkFormat format, const VkSampleCountFlagBits samplesCount, const uint32_t mipLevels, const uint32_t arrayLayers, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkImageCreateFlags flags, const VkMemoryPropertyFlags properties, VkImage& outImage, VkDeviceMemory& outImageMemory)
{
    VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    imageInfo.flags = flags;
    imageInfo.imageType = imageType;
    imageInfo.format = format;
    imageInfo.extent = { extent.width, extent.height, 1 };
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = arrayLayers;
    imageInfo.samples = samplesCount;
    imageInfo.tiling = tiling;
    imageInfo.usage = usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.queueFamilyIndexCount = 0;
    imageInfo.pQueueFamilyIndices = nullptr;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    VKERRCHECK(vkCreateImage(device, &imageInfo, nullptr, &outImage));

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, outImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(gpu, memRequirements.memoryTypeBits, properties);
    VKERRCHECK(vkAllocateMemory(device, &allocInfo, nullptr, &outImageMemory));

    VKERRCHECK(vkBindImageMemory(device, outImage, outImageMemory, 0));
}

VkImageView CreateImageView(const VkDevice device, const VkImage image, const VkFormat format, const VkImageViewType viewType, const uint32_t mipLevels, const VkImageAspectFlags aspectFlags, const uint32_t arrayLayers, const uint32_t baseArrayLayer)
{
    VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
    viewInfo.image = image;
    viewInfo.viewType = viewType;
    viewInfo.format = format;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = baseArrayLayer;
    viewInfo.subresourceRange.layerCount = arrayLayers;

    VkImageView imageView;
    VKERRCHECK(vkCreateImageView(device, &viewInfo, nullptr, &imageView));
    return imageView;
}

void TransitionImageLayout(const VkCommandBuffer commandBuffer, const VkImage image, const VkImageLayout oldLayout, const VkImageLayout newLayout, const uint32_t mipLevels, const VkImageAspectFlags aspectMask, const uint32_t layersCount)
{
    for (uint32_t layerIndex = 0; layerIndex < layersCount; ++layerIndex) {
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
        barrier.subresourceRange.aspectMask = aspectMask;

#if defined(__ANDROID__)
        VkPipelineStageFlags srcStage{ 0 };
        VkPipelineStageFlags dstStage{ 0 };
#else
        VkPipelineStageFlags srcStage{ VK_PIPELINE_STAGE_NONE };
        VkPipelineStageFlags dstStage{ VK_PIPELINE_STAGE_NONE };
#endif
        // old layout
        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED) {
            barrier.srcAccessMask = 0;
            srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT; // or VK_PIPELINE_STAGE_ALL_COMMANDS_BIT ??
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
            srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_GENERAL) {
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT; // | VK_ACCESS_HOST_WRITE_BIT
            srcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            srcStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        } else { // FULL barrier
            LOGW("WARN: performance - full barrier is used(src).\n");
            barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
            srcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        }

        // new layout
        if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        } else if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
            barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
            dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        } else if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        } else if (newLayout == VK_IMAGE_LAYOUT_GENERAL) {
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
            dstStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT; // or VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT ??
        } else if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL) {
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT; // TODO this might not be enough in certain cases
        } else { // FULL barrier
            LOGW("WARN: performance - full barrier is used(dst).\n");
            barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
            dstStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        }

        vkCmdPipelineBarrier(commandBuffer, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }
}

void GenerateMipmaps(const VkCommandBuffer commandBuffer, const VkImage image, const VkExtent3D& extent, const uint32_t mipLevels, const uint32_t layersCount, const VkImageAspectFlags aspectMask, const VkImageLayout newLayout)
{
    // TODO: this function uses internally linear filter -> it might not be supported for the desired imageFormat
    for (uint32_t layerIndex = 0; layerIndex < layersCount; ++layerIndex) {
        VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = aspectMask;
        barrier.subresourceRange.baseArrayLayer = layerIndex;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = extent.width;
        int32_t mipHeight = extent.height;
        int32_t mipDepth = extent.depth;

        for (uint32_t i = 1; i < mipLevels; ++i) {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

            VkImageBlit blit = {};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { mipWidth, mipHeight, mipDepth };
            blit.srcSubresource.aspectMask = aspectMask;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = layerIndex;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = { std::max(1, mipWidth / 2), std::max(1, mipHeight / 2), std::max(1, mipDepth / 2) };
            blit.dstSubresource.aspectMask = aspectMask;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = layerIndex;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = newLayout;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

            mipWidth = std::max(1, mipWidth / 2);
            mipHeight = std::max(1, mipHeight / 2);
            mipDepth = std::max(1, mipDepth / 2);
        }

        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = newLayout;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }
}

void CopyBufferToImage(const VkCommandBuffer commandBuffer, const VkExtent3D& extent, const VkBuffer buffer, const uint32_t layerIndex, const VkImageLayout dstImageLayout, VkImage dstImage)
{
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

    vkCmdCopyBufferToImage(commandBuffer, buffer, dstImage, dstImageLayout, 1, &region);
}

void CopyBuffer(const VkCommandBuffer commandBuffer, const VkBuffer srcBuffer, const VkDeviceSize size, VkBuffer dstBuffer)
{
    VkBufferCopy bufCopyRegion = {};
    bufCopyRegion.srcOffset = 0;
    bufCopyRegion.dstOffset = 0;
    bufCopyRegion.size = size;

    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &bufCopyRegion);
}

VkCommandPool CreateCommandPool(const VkDevice device, const uint32_t queueFamilyIndex)
{
    VkCommandPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
    poolInfo.queueFamilyIndex = queueFamilyIndex;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkCommandPool commandPool;
    VKERRCHECK(vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool));
    return commandPool;
}

VkFramebuffer CreateFrameBuffer(const VkDevice device, const VkRenderPass& renderPass, const std::vector<VkImageView>& imageViews, const VkExtent2D& extent)
{
    VkFramebuffer frameBuffer;

    VkFramebufferCreateInfo frameBufferCreateInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
    frameBufferCreateInfo.renderPass = renderPass;
    frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(imageViews.size());
    frameBufferCreateInfo.pAttachments = imageViews.data();
    frameBufferCreateInfo.width = extent.width;
    frameBufferCreateInfo.height = extent.height;
    frameBufferCreateInfo.layers = 1;
    VKERRCHECK(vkCreateFramebuffer(device, &frameBufferCreateInfo, nullptr, &frameBuffer));

    return frameBuffer;
}

VkCommandBuffer CreateCommandBuffer(const VkDevice device, const VkCommandPool commandPool, const VkCommandBufferLevel level)
{
    VkCommandBuffer commandBuffer;

    VkCommandBufferAllocateInfo commandBufferAllocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
    commandBufferAllocInfo.commandPool = commandPool;
    commandBufferAllocInfo.level = level;
    commandBufferAllocInfo.commandBufferCount = 1;
    VKERRCHECK(vkAllocateCommandBuffers(device, &commandBufferAllocInfo, &commandBuffer));

    return commandBuffer;
}

VkFence CreateFence(const VkDevice device, const VkFenceCreateFlags createFlags)
{
    VkFence fence;

    VkFenceCreateInfo createInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
    createInfo.flags = createFlags;
    VKERRCHECK(vkCreateFence(device, &createInfo, nullptr, &fence));

    return fence;
}

VkSemaphore CreateSemaphore(const VkDevice device, const VkSemaphoreCreateFlags createFlags)
{
    VkSemaphore semaphore;

    VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    createInfo.flags = createFlags;
    VKERRCHECK(vkCreateSemaphore(device, &createInfo, nullptr, &semaphore));

    return semaphore;
}

VkDescriptorSetLayoutBinding CreteDescriptorSetLayoutBinding(const uint32_t binding, const VkDescriptorType descType, const uint32_t descCount, const VkShaderStageFlags stageFlags, const VkSampler* immutableSamplers)
{
    VkDescriptorSetLayoutBinding vertexZeroLayoutBinding = {};
    vertexZeroLayoutBinding.binding = binding;
    vertexZeroLayoutBinding.descriptorType = descType;
    vertexZeroLayoutBinding.descriptorCount = descCount;
    vertexZeroLayoutBinding.stageFlags = stageFlags;
    vertexZeroLayoutBinding.pImmutableSamplers = immutableSamplers;
    return vertexZeroLayoutBinding;
}

VkWriteDescriptorSet CreateWriteDescriptorSet(const uint32_t dstBinding, const VkDescriptorType descType, const uint32_t descCount, const uint32_t dstArrayElement)
{
    VkWriteDescriptorSet vertexZeroWriteDescriptorSet = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
    vertexZeroWriteDescriptorSet.dstBinding = dstBinding;
    vertexZeroWriteDescriptorSet.descriptorType = descType;
    vertexZeroWriteDescriptorSet.descriptorCount = descCount;
    vertexZeroWriteDescriptorSet.dstArrayElement = dstArrayElement;
    return vertexZeroWriteDescriptorSet;
}

VkVertexInputBindingDescription CreateVertexInputBindingDescription(const uint32_t binding, const uint32_t stride, const VkVertexInputRate inputRate)
{
    VkVertexInputBindingDescription inputBinding = {};
    inputBinding.binding = binding;
    inputBinding.stride = stride;
    inputBinding.inputRate = inputRate;
    return inputBinding;
}

VkVertexInputAttributeDescription CreateVertexInputAttributeDescription(const uint32_t binding, const uint32_t location, const VkFormat format, const uint32_t offset)
{
    VkVertexInputAttributeDescription inputAttrDescription = {};
    inputAttrDescription.binding = binding;
    inputAttrDescription.location = location;
    inputAttrDescription.format = format;
    inputAttrDescription.offset = offset;
    return inputAttrDescription;
}

VkSampler CreateSampler(const VkDevice device, const float maxLod, const VkSamplerAddressMode addressMode, const VkFilter minFilter, const VkFilter magFilter, const VkSamplerMipmapMode mipMapMode, const bool enableAnisotropyFilter, const float maxAnisotropy)
{
    VkSampler sampler;

    VkSamplerCreateInfo samplerCreateInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
    if (enableAnisotropyFilter) {
        samplerCreateInfo.anisotropyEnable = VK_TRUE;
        samplerCreateInfo.maxAnisotropy = maxAnisotropy;
    } else {
        samplerCreateInfo.anisotropyEnable = VK_FALSE;
        samplerCreateInfo.maxAnisotropy = 1.0;
    }

    samplerCreateInfo.minFilter = minFilter;
    samplerCreateInfo.magFilter = magFilter;
    samplerCreateInfo.mipmapMode = mipMapMode;

    samplerCreateInfo.addressModeU = addressMode;
    samplerCreateInfo.addressModeV = addressMode;
    samplerCreateInfo.addressModeW = addressMode;
    samplerCreateInfo.mipLodBias = 0.0f;

    samplerCreateInfo.compareEnable = VK_FALSE;
    samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerCreateInfo.minLod = 0.0f;
    samplerCreateInfo.maxLod = maxLod;
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
    VKERRCHECK(vkCreateSampler(device, &samplerCreateInfo, nullptr, &sampler));

    return sampler;
}

VkSampleCountFlagBits GetMaxUsableSampleCount(const VkPhysicalDevice physicalDevice)
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    const VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    const VkSampleCountFlagBits bits[] = {
        VK_SAMPLE_COUNT_64_BIT,
        VK_SAMPLE_COUNT_32_BIT,
        VK_SAMPLE_COUNT_16_BIT,
        VK_SAMPLE_COUNT_8_BIT,
        VK_SAMPLE_COUNT_4_BIT,
        VK_SAMPLE_COUNT_2_BIT
    };

    for (const auto& bit : bits) {
        if (counts & bit) {
            return bit;
        }
    }
    return VK_SAMPLE_COUNT_1_BIT;
}

VkSampleCountFlagBits GetSampleCountBit(const uint32_t sampleCount)
{
    switch (sampleCount) {
    case 2:
        return VK_SAMPLE_COUNT_2_BIT;
    case 4:
        return VK_SAMPLE_COUNT_4_BIT;
    case 8:
        return VK_SAMPLE_COUNT_8_BIT;
    case 16:
        return VK_SAMPLE_COUNT_16_BIT;
    case 32:
        return VK_SAMPLE_COUNT_32_BIT;
    case 64:
        return VK_SAMPLE_COUNT_64_BIT;
    default:
        return VK_SAMPLE_COUNT_1_BIT;
    }
}

std::string FormatToString(const VkFormat fmt)
{
    switch (fmt) {
    case VK_FORMAT_R5G6B5_UNORM_PACK16:
        return STRINGIFY(VK_FORMAT_R5G6B5_UNORM_PACK16); //  4
    case VK_FORMAT_R8G8B8A8_UNORM:
        return STRINGIFY(VK_FORMAT_R8G8B8A8_UNORM); // 37
    case VK_FORMAT_R8G8B8A8_SRGB:
        return STRINGIFY(VK_FORMAT_R8G8B8A8_SRGB); // 43
    case VK_FORMAT_B8G8R8A8_UNORM:
        return STRINGIFY(VK_FORMAT_B8G8R8A8_UNORM); // 44
    case VK_FORMAT_B8G8R8A8_SRGB:
        return STRINGIFY(VK_FORMAT_B8G8R8A8_SRGB); // 50
        // Depth
    case VK_FORMAT_D32_SFLOAT:
        return STRINGIFY(VK_FORMAT_D32_SFLOAT); // 126
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
        return STRINGIFY(VK_FORMAT_D32_SFLOAT_S8_UINT); // 130
    case VK_FORMAT_D24_UNORM_S8_UINT:
        return STRINGIFY(VK_FORMAT_D24_UNORM_S8_UINT); // 129
    case VK_FORMAT_D16_UNORM_S8_UINT:
        return STRINGIFY(VK_FORMAT_D16_UNORM_S8_UINT); // 128
    case VK_FORMAT_D16_UNORM:
        return STRINGIFY(VK_FORMAT_D16_UNORM); // 124
    default:
        return STRINGIFY(VK_FORMAT_UNDEFINED);
    }
}

std::string PresentModeToString(const VkPresentModeKHR mode)
{
    switch (mode) {
    case VK_PRESENT_MODE_IMMEDIATE_KHR:
        return STRINGIFY(VK_PRESENT_MODE_IMMEDIATE_KHR);
    case VK_PRESENT_MODE_MAILBOX_KHR:
        return STRINGIFY(VK_PRESENT_MODE_MAILBOX_KHR);
    case VK_PRESENT_MODE_FIFO_KHR:
        return STRINGIFY(VK_PRESENT_MODE_FIFO_KHR);
    case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
        return STRINGIFY(VK_PRESENT_MODE_FIFO_RELAXED_KHR);
    case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
        return STRINGIFY(VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR);
    case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
        return STRINGIFY(VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR);
    default:
        return "UNKNOWN_PRESENT_MODE";
    }
}

uint32_t GetComputeGroupSize(const uint32_t val, const uint32_t blockSize)
{
    return std::max((val + blockSize - 1) / blockSize, 1u);
}

std::string QueueFlagsToString(const VkQueueFlags quueFlags)
{
    static const std::map<uint32_t, std::string> flagsNames = {
        { 0x1, "GRAPHICS" },
        { 0x2, "COMPUTE" },
        { 0x4, "TRANSFER" },
        { 0x8, "SPARSE" },
        { 0x10, "PROTECTED" },
        { 0x20, "VIDEO_DECODE" },
        { 0x40, "VIDEO_ENCODE" },
        { 0x100, "OPTICAL_FLOW" }
    };

    std::stringstream ss;
    for (const auto& [flags, name] : flagsNames) {
        if (quueFlags & flags) {
            ss << name << " ";
        }
    }
    return ss.str();
}

std::string VendorIdToString(const uint32_t vendorId)
{
    switch (vendorId) {
    case 0x1002:
        return "AMD";
    case 0x8086:
        return "INTEL";
    case 0x10DE:
        return "NVIDIA";
    case 0x13B5:
        return "ARM";
    case 0x5143:
        return "Qualcomm";
    case 0x1010:
        return "Imagination";
    case 0x106B:
        return "Apple";
    default:
        return "UNKNOWN";
    }
}

VkFormatProperties GetFormatProperties(const VkPhysicalDevice physicalDevice, const VkFormat format)
{
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);
    return formatProperties;
}

} // namespace prev::util::vk
