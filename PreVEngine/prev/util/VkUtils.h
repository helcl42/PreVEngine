#ifndef __VK_UTILS_H__
#define __VK_UTILS_H__

#include "../common/Common.h"
#include "../core/Core.h"

#include <vector>

#undef CreateSemaphore

namespace prev::util::vk {

uint32_t FindMemoryType(const VkPhysicalDevice gpu, const uint32_t typeFilter, const VkMemoryPropertyFlags properties);

void CreateImage(const VkPhysicalDevice gpu, const VkDevice device, const VkExtent2D& extent, const VkImageType imageType, const VkFormat format, const VkSampleCountFlagBits samplesCount, const uint32_t mipLevels, const uint32_t arrayLayers, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkImageCreateFlags flags, const VkMemoryPropertyFlags properties, VkImage& outImage, VkDeviceMemory& outImageMemory);

VkImageView CreateImageView(const VkDevice device, const VkImage image, const VkFormat format, const VkImageViewType viewType, const uint32_t mipLevels, const VkImageAspectFlags aspectFlags, const uint32_t arrayLayers = 1, const uint32_t baseArrayLayer = 0);

void TransitionImageLayout(const VkCommandBuffer commandBuffer, const VkImage image, const VkImageLayout oldLayout, const VkImageLayout newLayout, const uint32_t mipLevels, const VkImageAspectFlags aspectMask, const uint32_t layersCount);

void GenerateMipmaps(const VkCommandBuffer commandBuffer, const VkImage image, const VkExtent3D& extent, const uint32_t mipLevels, const uint32_t layersCount, const VkImageAspectFlags aspectMask, const VkFilter filter, const VkImageLayout layout);

void CopyBufferToImage(const VkCommandBuffer commandBuffer, const VkExtent3D& extent, const VkBuffer buffer, const uint32_t layerIndex, const VkImageLayout dstImageLayout, VkImage dstImage);

void CopyBuffer(const VkCommandBuffer commandBuffer, const VkBuffer srcBuffer, const VkDeviceSize size, VkBuffer dstBuffer);

void CopyImage(const VkCommandBuffer commandBuffer, const VkImage srcImage, const VkExtent3D& extent, const uint32_t layersCount, const VkImageAspectFlags aspectMask, const VkFilter filter, const VkImageLayout layout, VkImage dstImage);

VkFramebuffer CreateFrameBuffer(const VkDevice device, const VkRenderPass& renderPass, const std::vector<VkImageView>& imageViews, const VkExtent2D& extent);

VkCommandPool CreateCommandPool(const VkDevice device, const uint32_t queueFamilyIndex);

VkCommandBuffer CreateCommandBuffer(const VkDevice device, const VkCommandPool commandPool, const VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

VkFence CreateFence(const VkDevice device, const VkFenceCreateFlags createFlags = VK_FENCE_CREATE_SIGNALED_BIT);

VkSemaphore CreateSemaphore(const VkDevice device, const VkSemaphoreCreateFlags createFlags = 0);

VkDescriptorSetLayoutBinding CreteDescriptorSetLayoutBinding(const uint32_t binding, const VkDescriptorType descType, const uint32_t descCount, const VkShaderStageFlags stageFlags, const VkSampler* immutableSamplers = nullptr);

VkWriteDescriptorSet CreateWriteDescriptorSet(const uint32_t dstBinding, const VkDescriptorType descType, const uint32_t descCount, const uint32_t dstArrayElement = 0);

std::vector<VkDescriptorSet> CreateDescriptorSets(const VkDevice device, const uint32_t size, const VkDescriptorPool descriptorPool, const VkDescriptorSetLayout descriptorSetLayout);

VkDescriptorPool CreateDescriptorPool(const VkDevice device, const uint32_t size, const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings);

void DestroyDescriptorPool(const VkDevice device, VkDescriptorPool descriptorPool);

VkVertexInputBindingDescription CreateVertexInputBindingDescription(const uint32_t binding, const uint32_t stride, const VkVertexInputRate inputRate);

VkVertexInputAttributeDescription CreateVertexInputAttributeDescription(const uint32_t binding, const uint32_t location, const VkFormat format, const uint32_t offset);

VkSampleCountFlagBits GetMaxUsableSampleCount(const VkPhysicalDevice physicalDevice);

VkSampleCountFlagBits GetSampleCountBit(const uint32_t sampleCount);

std::string FormatToString(const VkFormat fmt);

std::string PresentModeToString(const VkPresentModeKHR mode);

uint32_t GetComputeGroupSize(const uint32_t val, const uint32_t blockSize);

std::string QueueFlagsToString(const VkQueueFlags flags);

std::string VendorIdToString(const uint32_t vendorId);

VkFormatProperties GetFormatProperties(const VkPhysicalDevice physicalDevice, const VkFormat format);

VkFormat ToImageFormat(const uint32_t channels, const uint32_t bitDepth, const bool isFloatingPoint);

} // namespace prev::util::vk

#endif
