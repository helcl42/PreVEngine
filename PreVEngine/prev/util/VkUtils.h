#ifndef __VK_UTILS_H__
#define __VK_UTILS_H__

#include "../common/Common.h"
#include "../core/instance/Validation.h"

namespace prev::util::vk {

uint32_t FindMemoryType(const VkPhysicalDevice gpu, const uint32_t typeFilter, const VkMemoryPropertyFlags properties);

void CreateImage(const VkPhysicalDevice gpu, const VkDevice device, const VkExtent2D& extent, const VkImageType imageType, const VkFormat format, const VkSampleCountFlagBits samplesCount, const uint32_t mipLevels, const uint32_t arrayLayers, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkImageCreateFlags flags, const VkMemoryPropertyFlags properties, VkImage& outImage, VkDeviceMemory& outImageMemory);

VkImageView CreateImageView(const VkDevice device, const VkImage image, const VkFormat format, const VkImageViewType viewType, const uint32_t mipLevels, const VkImageAspectFlags aspectFlags, const uint32_t arrayLayers = 1, const uint32_t baseArrayLayer = 0);

VkFramebuffer CreateFrameBuffer(const VkDevice device, const VkRenderPass& renderPass, const std::vector<VkImageView>& imageViews, const VkExtent2D& extent);

VkCommandBuffer CreateCommandBuffer(const VkDevice device, const VkCommandPool commandPool, const VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

VkFence CreateFence(const VkDevice device, const VkFenceCreateFlags createFlags = VK_FENCE_CREATE_SIGNALED_BIT);

VkDescriptorSetLayoutBinding CreteDescriptorSetLayoutBinding(const uint32_t binding, const VkDescriptorType descType, const uint32_t descCount, const VkShaderStageFlags stageFlags, const VkSampler* immutableSamplers = nullptr);

VkWriteDescriptorSet CreateWriteDescriptorSet(const uint32_t dstBinding, const VkDescriptorType descType, const uint32_t descCount, const uint32_t dstArrayElement = 0);

VkVertexInputBindingDescription CreateVertexInputBindingDescription(const uint32_t binding, const uint32_t stride, const VkVertexInputRate inputRate);

VkVertexInputAttributeDescription CreateVertexInputAttributeDescription(const uint32_t binding, const uint32_t location, const VkFormat format, const uint32_t offset);

VkSampler CreateSampler(const VkDevice device, const float maxLod, const VkSamplerAddressMode addressMode, const VkFilter minFilter, const VkFilter magFilter, const VkSamplerMipmapMode mipMapMode, const bool enableAnisotropyFilter = false, const float maxAnisotropy = 16.0f);

VkSampleCountFlagBits GetMaxUsableSampleCount(const VkPhysicalDevice physicalDevice);

VkSampleCountFlagBits GetSampleCountBit(const uint32_t sampleCount);

std::string FormatToString(const VkFormat fmt);

std::string PresentModeToString(const VkPresentModeKHR mode);

} // namespace prev::util::vk

#endif