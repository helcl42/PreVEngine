#include "VkUtils.h"

#include <limits>

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

    LOGE("failed to find suitable memory type!");
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
    const auto div{ val / blockSize };
    if (val % blockSize == 0) {
        return div;
    } else {
        return div + blockSize;
    }
}
} // namespace prev::util::vk
