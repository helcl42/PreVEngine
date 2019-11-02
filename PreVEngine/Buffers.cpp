#include "Buffers.h"
#include "Formats.h"

#define NOMINMAX
//#define VMA_RECORDING_ENABLED   0
//#define VMA_DEDICATED_ALLOCATION   0
//#define VMA_STATS_STRING_ENABLED   1
#define VMA_STATIC_VULKAN_FUNCTIONS 0

#define VMA_IMPLEMENTATION
#include "External/vk_mem_alloc.h"

#include <math.h>

static uint32_t Log2(uint32_t x)
{
	return (uint32_t)(log(x) / log(2));
}

static uint32_t FindMemoryType(VkPhysicalDevice gpu, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(gpu, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	LOGE("failed to find suitable memory type!");

	return 0;
}

static void CreateImage(VkPhysicalDevice gpu, VkDevice device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
	VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.format = format;  //VK_FORMAT_D32_SFLOAT
	imageInfo.extent = { width, height, 1 };
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling = tiling;  //VK_IMAGE_TILING_OPTIMAL
	imageInfo.usage = usage;    //VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.queueFamilyIndexCount = 0;
	imageInfo.pQueueFamilyIndices = nullptr;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	VKERRCHECK(vkCreateImage(device, &imageInfo, nullptr, &image));

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(gpu, memRequirements.memoryTypeBits, properties);
	VKERRCHECK(vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory));
	VKERRCHECK(vkBindImageMemory(device, image, imageMemory, 0));
}

static VkImageView CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
	viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
	viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
	viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	VKERRCHECK(vkCreateImageView(device, &viewInfo, nullptr, &imageView));
	return imageView;
}

//------------------------------------------------------------------------------------------------

DepthBuffer::DepthBuffer() 
	: m_gpu(VK_NULL_HANDLE), m_device(VK_NULL_HANDLE), m_format(VK_FORMAT_UNDEFINED), m_image(VK_NULL_HANDLE), m_imageMemory(VK_NULL_HANDLE), m_imageView(VK_NULL_HANDLE)
{
}
DepthBuffer::~DepthBuffer()
{
	Destroy();
}

void DepthBuffer::Create(VkPhysicalDevice gpu, VkDevice device, VkExtent2D extent, VkFormat format)
{
	this->m_gpu = gpu;
	this->m_device = device;
	this->m_format = format;

	Resize(extent);
}

void DepthBuffer::Destroy()
{
	if (m_imageView)
	{
		vkDestroyImageView(m_device, m_imageView, nullptr);
	}

	if (m_image)
	{
		vkDestroyImage(m_device, m_image, nullptr);
	}

	if (m_imageMemory)
	{
		vkFreeMemory(m_device, m_imageMemory, nullptr);
	}
}

void DepthBuffer::Resize(VkExtent2D extent)
{
	Destroy();
	
	if (m_format == VK_FORMAT_UNDEFINED)
	{
		return;
	}

	CreateImage(m_gpu, m_device, extent.width, extent.height, m_format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_image, m_imageMemory);
	
	m_imageView = CreateImageView(m_device, m_image, m_format, VK_IMAGE_ASPECT_DEPTH_BIT);
}

VkFormat DepthBuffer::GetFormat() const
{
	return m_format;
}

VkImage DepthBuffer::GetImage() const
{
	return m_image;
}

VkImageView DepthBuffer::GetImageView() const
{
	return m_imageView;
}


Allocator::Allocator(const Queue& q, VkDeviceSize blockSize) 
	: m_allocator(VK_NULL_HANDLE)
{
	m_gpu = q.gpu;
	m_device = q.device;
	m_queue = q.handle;
	m_commandPool = q.CreateCommandPool();
	m_commandBuffer = q.CreateCommandBuffer(m_commandPool);

	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = m_gpu;
	allocatorInfo.device = m_device;
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
	fn.vkGetBufferMemoryRequirements2KHR = 0;  //(PFN_vkGetBufferMemoryRequirements2KHR)vkGetBufferMemoryRequirements2KHR;
	fn.vkGetImageMemoryRequirements2KHR = 0;  //(PFN_vkGetImageMemoryRequirements2KHR)vkGetImageMemoryRequirements2KHR;
	allocatorInfo.pVulkanFunctions = &fn;

	VKERRCHECK(vmaCreateAllocator(&allocatorInfo, &m_allocator));
}

Allocator::~Allocator()
{
	if (m_commandBuffer)
	{
		vkFreeCommandBuffers(m_device, m_commandPool, 1, &m_commandBuffer);
	}
	
	if (m_commandPool)
	{
		vkDestroyCommandPool(m_device, m_commandPool, nullptr);
	}

	if (m_allocator)
	{
		vmaDestroyAllocator(m_allocator);
	}
}

void Allocator::BeginCmd()
{
	VkCommandBufferBeginInfo cmdBufBeginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	cmdBufBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	VKERRCHECK(vkBeginCommandBuffer(m_commandBuffer, &cmdBufBeginInfo));
}

void Allocator::EndCmd()
{
	VKERRCHECK(vkEndCommandBuffer(m_commandBuffer));
	VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_commandBuffer;
	VKERRCHECK(vkQueueSubmit(m_queue, 1, &submitInfo, VK_NULL_HANDLE));
	VKERRCHECK(vkQueueWaitIdle(m_queue));
}

void Allocator::CreateBuffer(const void* data, uint64_t size, VkBufferUsageFlags usage, VmaMemoryUsage memtype, VkBuffer& buffer, VmaAllocation& alloc, void** mapped)
{
	VkBufferCreateInfo bufInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufInfo.size = size;
	bufInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocCreateInfo = {};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
	allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

	VmaAllocationInfo allocInfo = {};

	// For CPU-visible memory, skip staging buffer
	if (memtype != VMA_MEMORY_USAGE_GPU_ONLY)
	{
		bufInfo.usage = usage;
		VKERRCHECK(vmaCreateBuffer(m_allocator, &bufInfo, &allocCreateInfo, &buffer, &alloc, &allocInfo));
		if (data)
		{
			memcpy(allocInfo.pMappedData, data, size);
		}
		else
		{
			memset(allocInfo.pMappedData, 0, size);
		}

		if (mapped)
		{
			*mapped = allocInfo.pMappedData;
		}

		return;
	}

	// For GPU-only memory, copy via staging buffer.  // TODO: Also skip staging buffer on integrated gpus.
	VkBuffer stageBuffer = VK_NULL_HANDLE;
	VmaAllocation stageBufferAlloc = VK_NULL_HANDLE;

	VKERRCHECK(vmaCreateBuffer(m_allocator, &bufInfo, &allocCreateInfo, &stageBuffer, &stageBufferAlloc, &allocInfo));
	memcpy(allocInfo.pMappedData, data, size);

	bufInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage;  // | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	allocCreateInfo.usage = memtype;                           // VMA_MEMORY_USAGE_GPU_ONLY;
	allocCreateInfo.flags = 0;
	VKERRCHECK(vmaCreateBuffer(m_allocator, &bufInfo, &allocCreateInfo, &buffer, &alloc, nullptr));

	BeginCmd();
	VkBufferCopy bufCopyRegion = {};
	bufCopyRegion.srcOffset = 0;
	bufCopyRegion.dstOffset = 0;
	bufCopyRegion.size = bufInfo.size;
	vkCmdCopyBuffer(m_commandBuffer, stageBuffer, buffer, 1, &bufCopyRegion);
	EndCmd();

	vmaDestroyBuffer(m_allocator, stageBuffer, stageBufferAlloc);
}

void Allocator::DestroyBuffer(VkBuffer buffer, VmaAllocation alloc)
{
	vmaDestroyBuffer(m_allocator, buffer, alloc);
}

void Allocator::CreateImage(const void* data, VkExtent3D extent, VkFormat format, uint32_t mipLevels, VkImage& image, VmaAllocation& alloc, VkImageView& view)
{
	uint32_t fmt_size = FormatSize(format);

	// Copy image data to staging buffer in CPU memory
	uint64_t size = extent.width * extent.height * extent.depth * fmt_size;

	VkBufferCreateInfo bufInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufInfo.size = size;
	bufInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocCreateInfo = {};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
	allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

	VmaAllocationInfo allocInfo = {};
	VkBuffer stageBuffer = VK_NULL_HANDLE;
	VmaAllocation stageBufferAlloc = VK_NULL_HANDLE;
	VKERRCHECK(vmaCreateBuffer(m_allocator, &bufInfo, &allocCreateInfo, &stageBuffer, &stageBufferAlloc, &allocInfo));
	memcpy(allocInfo.pMappedData, data, size);

	// Create image in GPU memory
	//uint32_t mipLevels = 1;
	//if(mipmap) mipLevels = (uint32_t)(Log2(std::max(extent.width, extent.height))) + 1;

	VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	imageInfo.flags = 0;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.format = format; //VK_FORMAT_R8G8B8A8_UNORM;
	imageInfo.extent = extent;
	imageInfo.mipLevels = mipLevels; //1;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	//imageInfo.queueFamilyIndexCount = 0;
	//imageInfo.pQueueFamilyIndices = 0;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;  //memtype
	allocCreateInfo.flags = 0;
	vmaCreateImage(m_allocator, &imageInfo, &allocCreateInfo, &image, &alloc, nullptr);

	//  Copy image from staging buffer to image
	BeginCmd();

	TransitionImageLayout(image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = extent;  // { width, height, 1};
	vkCmdCopyBufferToImage(m_commandBuffer, stageBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	if (mipLevels <= 1)
	{
		TransitionImageLayout(image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels);
	}

	EndCmd();

	vmaDestroyBuffer(m_allocator, stageBuffer, stageBufferAlloc);

	if (mipLevels > 1)
	{
		GenerateMipmaps(image, format, extent.width, extent.height, mipLevels);
	}

	// Create ImageView
	VkImageViewCreateInfo imageViewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	imageViewInfo.image = image;
	imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewInfo.format = format;  //VK_FORMAT_R8G8B8A8_UNORM;
	imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageViewInfo.subresourceRange.baseMipLevel = 0;
	imageViewInfo.subresourceRange.levelCount = mipLevels; //1;
	imageViewInfo.subresourceRange.baseArrayLayer = 0;
	imageViewInfo.subresourceRange.layerCount = 1;
	VKERRCHECK(vkCreateImageView(m_device, &imageViewInfo, nullptr, &view));
}

void Allocator::GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
	// Check if image format supports linear blitting
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(m_gpu, imageFormat, &formatProperties);

	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
	{
		LOGE("Texture image format does not support linear blitting!");
		exit(0);
	}

	//VkCommandBuffer command_buffer = beginSingleTimeCommands();
	BeginCmd();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = image;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	int32_t mipWidth = texWidth;
	int32_t mipHeight = texHeight;

	for (uint32_t i = 1; i < mipLevels; i++)
	{
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(m_commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		VkImageBlit blit = {};
		blit.srcOffsets[0] = { 0, 0, 0 };
		blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(m_commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(m_commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;
	}

	barrier.subresourceRange.baseMipLevel = mipLevels - 1;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(m_commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

	//endSingleTimeCommands(command_buffer);
	EndCmd();
}

void Allocator::TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
{
	//BeginCmd();
	VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;  // NOTE: subresourceRange is same as the one in ImageView
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = mipLevels; //1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags srcStage;
	VkPipelineStageFlags dstStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	else
	{
		LOGE("Unsupported layout transition\n");
	}

	vkCmdPipelineBarrier(m_commandBuffer, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	//EndCmd();
}

void Allocator::DestroyImage(VkImage image, VkImageView view, VmaAllocation alloc)
{
	if (view)
	{
		vkDestroyImageView(m_device, view, nullptr);
	}

	vmaDestroyImage(m_allocator, image, alloc);
}

VkPhysicalDevice Allocator::GetPhysicalDevice() const
{
	return m_gpu;
}

VkDevice Allocator::GetDevice() const
{
	return m_device;
}

VkQueue Allocator::GetQueue() const
{
	return m_queue;
}


Buffer::Buffer(Allocator& allocator) 
	: m_allocator(&allocator), m_allocation(), m_buffer(), m_count(), m_stride()
{
}

Buffer::~Buffer()
{
	Clear();
}

void Buffer::Clear()
{
	vkQueueWaitIdle(m_allocator->GetQueue());
	
	if (m_buffer)
	{
		m_allocator->DestroyBuffer(m_buffer, m_allocation);
	}

	m_buffer = 0;
	m_count = 0;
	m_stride = 0;
}

void Buffer::Data(const void* data, uint32_t count, uint32_t stride, VkBufferUsageFlagBits usage, VmaMemoryUsage memtype, void** mapped)
{
	Clear();

	m_allocator->CreateBuffer(data, count * stride, usage, memtype, m_buffer, m_allocation, mapped);
	
	if (!m_buffer)
	{
		return;
	}

	m_count = count;
	m_stride = stride;
}

uint32_t Buffer::GetCount() const
{
	return m_count;
}

Buffer::operator VkBuffer() const
{
	return m_buffer;
}


void VBO::Data(void* data, uint32_t count, uint32_t stride)
{
	Buffer::Data(data, count, stride, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
}
//----------------------------------------------------

//--------------------------IBO-----------------------
void IBO::Data(const uint16_t* data, uint32_t count)
{
	Buffer::Data(data, count, 2, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
}

void IBO::Data(const uint32_t* data, uint32_t count)
{
	Buffer::Data(data, count, 4, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
}



//void UBO::Data(void* data, uint32_t size) 
//{
//	Buffer::Data(data, 1, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, &m_mapped);
//}

void UBO::Allocate(uint32_t size)
{
	Buffer::Data(0, 1, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, &m_mapped);
}

void UBO::Update(void* data)
{
	memcpy(m_mapped, data, m_stride);
}

uint32_t UBO::GetSize() const
{
	return m_stride;
}


ImageBuffer::ImageBuffer(Allocator& allocator) 
	: m_allocator(&allocator), m_allocation(), m_image(), m_imageView(), m_sampler(), m_extent(), m_format()
{
}

ImageBuffer::~ImageBuffer()
{
	Clear();
}

void ImageBuffer::Clear()
{
	vkQueueWaitIdle(m_allocator->GetQueue());

	if (m_sampler)
	{
		vkDestroySampler(m_allocator->GetDevice(), m_sampler, nullptr);
	}

	if (m_image)
	{
		m_allocator->DestroyImage(m_image, m_imageView, m_allocation);
	}

	m_image = 0;
	m_extent = {};
	m_format = VK_FORMAT_UNDEFINED;
}

void ImageBuffer::Data(const void* data, VkExtent3D extent, VkFormat format, bool mipmap)
{
	Clear();

	uint32_t mipLevels = 1;
	if (mipmap)
	{
		mipLevels = Log2(std::max(extent.width, extent.height)) + 1;
	}

	m_allocator->CreateImage(data, extent, format, mipLevels, m_image, m_allocation, m_imageView);

	if (!m_image)
	{
		return;
	}
	
	m_format = format;

	CreateSampler((float)mipLevels);
}

void ImageBuffer::CreateSampler(float maxLod)
{
	if (m_sampler)
	{
		vkDestroySampler(m_allocator->GetDevice(), m_sampler, nullptr);
	}
	
	VkSamplerCreateInfo samplerInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.mipLodBias = 0;

	//samplerInfo.anisotropyEnable = VK_TRUE;
	//samplerInfo.maxAnisotropy = 16;
	samplerInfo.anisotropyEnable = VK_FALSE;
	samplerInfo.maxAnisotropy = 1;

	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.minLod = 0;
	samplerInfo.maxLod = maxLod; //mipLevels;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	VKERRCHECK(vkCreateSampler(m_allocator->GetDevice(), &samplerInfo, nullptr, &m_sampler));
}

void ImageBuffer::UpdateSampler(VkSamplerCreateInfo& samplerInfo)
{
	if (m_sampler)
	{
		vkDestroySampler(m_allocator->GetDevice(), m_sampler, nullptr);
	}

	VKERRCHECK(vkCreateSampler(m_allocator->GetDevice(), &samplerInfo, nullptr, &m_sampler));
}

VkImage ImageBuffer::GetImage() const
{
	return m_image;
}

VkImageView ImageBuffer::GetImageView() const
{
	return m_imageView;
}

VkSampler ImageBuffer::GetSampler() const
{
	return m_sampler;
}

VkFormat ImageBuffer::GetFormat() const
{
	return m_format;
}

VkExtent2D ImageBuffer::GetExtent() const
{
	return m_extent;
}
