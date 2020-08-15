#ifndef __VK_UTILS_H__
#define __VK_UTILS_H__

#include "../common/Common.h"
#include "../core/instance/Validation.h"

namespace prev {
class VkUtils {
public:
    static uint32_t FindMemoryType(const VkPhysicalDevice gpu, const uint32_t typeFilter, const VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(gpu, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        LOGE("failed to find suitable memory type!");

        return 0;
    }

    static void CreateImage(const VkPhysicalDevice gpu, const VkDevice device, const VkExtent2D& extent, const VkImageType imageType, const VkFormat format, const uint32_t mipLevels, const uint32_t arrayLayers, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkImageCreateFlags flags, const VkMemoryPropertyFlags properties, VkImage& outImage, VkDeviceMemory& outImageMemory)
    {
        VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        imageInfo.flags = flags;
        imageInfo.imageType = imageType;
        imageInfo.format = format;
        imageInfo.extent = { extent.width, extent.height, 1 };
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = arrayLayers;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
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

    static VkImageView CreateImageView(const VkDevice device, const VkImage image, const VkFormat format, const VkImageViewType viewType, const uint32_t mipLevels, const VkImageAspectFlags aspectFlags, const uint32_t arrayLayers = 1, const uint32_t baseArrayLayer = 0)
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

    static VkFramebuffer CreateFrameBuffer(const VkDevice device, const VkRenderPass& renderPass, const std::vector<VkImageView>& imageViews, const VkExtent2D& extent)
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

    static VkCommandBuffer CreateCommandBuffer(const VkDevice device, const VkCommandPool commandPool, const VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY)
    {
        VkCommandBuffer commandBuffer;

        VkCommandBufferAllocateInfo commandBufferAllocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        commandBufferAllocInfo.commandPool = commandPool;
        commandBufferAllocInfo.level = level;
        commandBufferAllocInfo.commandBufferCount = 1;
        VKERRCHECK(vkAllocateCommandBuffers(device, &commandBufferAllocInfo, &commandBuffer));

        return commandBuffer;
    }

    static VkFence CreateFence(const VkDevice device, const VkFenceCreateFlags createFlags = VK_FENCE_CREATE_SIGNALED_BIT)
    {
        VkFence fence;

        VkFenceCreateInfo createInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
        createInfo.flags = createFlags;
        VKERRCHECK(vkCreateFence(device, &createInfo, nullptr, &fence));

        return fence;
    }

    static VkDescriptorSetLayoutBinding CreteDescriptorSetLayoutBinding(const uint32_t binding, const VkDescriptorType descType, const uint32_t descCount, const VkShaderStageFlags stageFlags, const VkSampler* immutableSamplers = nullptr)
    {
        VkDescriptorSetLayoutBinding vertexZeroLayoutBinding = {};
        vertexZeroLayoutBinding.binding = binding;
        vertexZeroLayoutBinding.descriptorType = descType;
        vertexZeroLayoutBinding.descriptorCount = descCount;
        vertexZeroLayoutBinding.stageFlags = stageFlags;
        vertexZeroLayoutBinding.pImmutableSamplers = immutableSamplers;
        return vertexZeroLayoutBinding;
    }

    static VkWriteDescriptorSet CreateWriteDescriptorSet(const uint32_t dstBinding, const VkDescriptorType descType, const uint32_t descCount, const uint32_t dstArrayElement = 0)
    {
        VkWriteDescriptorSet vertexZeroWriteDescriptorSet = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        vertexZeroWriteDescriptorSet.dstBinding = dstBinding;
        vertexZeroWriteDescriptorSet.descriptorType = descType;
        vertexZeroWriteDescriptorSet.descriptorCount = descCount;
        vertexZeroWriteDescriptorSet.dstArrayElement = dstArrayElement;
        return vertexZeroWriteDescriptorSet;
    }

    static VkVertexInputBindingDescription CreateVertexInputBindingDescription(const uint32_t binding, const uint32_t stride, const VkVertexInputRate inputRate)
    {
        VkVertexInputBindingDescription inputBinding = {};
        inputBinding.binding = binding;
        inputBinding.stride = stride;
        inputBinding.inputRate = inputRate;
        return inputBinding;
    }

    static VkVertexInputAttributeDescription CreateVertexInputAttributeDescription(const uint32_t binding, const uint32_t location, const VkFormat format, const uint32_t offset)
    {
        VkVertexInputAttributeDescription inputAttrDescription = {};
        inputAttrDescription.binding = binding;
        inputAttrDescription.location = location;
        inputAttrDescription.format = format;
        inputAttrDescription.offset = offset;
        return inputAttrDescription;
    }
};
}

#endif