#include "Buffers.h"
#include "Formats.h"
#include "Utils.h"

#define NOMINMAX
//#define VMA_RECORDING_ENABLED   0
//#define VMA_DEDICATED_ALLOCATION   0
//#define VMA_STATS_STRING_ENABLED   1
#define VMA_STATIC_VULKAN_FUNCTIONS 0

#define VMA_IMPLEMENTATION
#include "External/vk_mem_alloc.h"

namespace PreVEngine
{
	Allocator::Allocator(const Queue& q, const VkDeviceSize blockSize)
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
		VKERRCHECK(vkQueueSubmit(m_queue, 1, &submitInfo, VK_NULL_HANDLE));
		VKERRCHECK(vkQueueWaitIdle(m_queue));
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
		}
		else // For GPU-only memory, copy via staging buffer.  
		{
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
			bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage;  // | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

			VmaAllocationCreateInfo allocCreateInfo = {};
			allocCreateInfo.usage = memtype;
			allocCreateInfo.flags = 0;
			VKERRCHECK(vmaCreateBuffer(m_allocator, &bufferCreateInfo, &allocCreateInfo, &buffer, &alloc, nullptr));

			CopyBuffer(stageBuffer, bufferCreateInfo.size, buffer);

			vmaDestroyBuffer(m_allocator, stageBuffer, stageBufferAlloc);
		}
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

	void Allocator::CopyBufferToImage(const VkExtent3D& extent, const VkBuffer buffer, VkImage image)
	{
		BeginCommandBuffer();

		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = extent;

		vkCmdCopyBufferToImage(m_commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		EndCommandBuffer();
	}

	void Allocator::DestroyBuffer(VkBuffer buffer, VmaAllocation alloc)
	{
		vmaDestroyBuffer(m_allocator, buffer, alloc);
	}

	void Allocator::CreateImage(const VkExtent3D& extent, const VkFormat format, const uint32_t mipLevels, const VkImageTiling tiling, const VkImageUsageFlags usage, VkImage& outImage, VmaAllocation& outAlloc)
	{
		VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		imageInfo.flags = 0;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.format = format;
		imageInfo.extent = extent;
		imageInfo.mipLevels = mipLevels;
		imageInfo.arrayLayers = 1;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.tiling = tiling;
		imageInfo.usage = usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VmaAllocationCreateInfo allocGpuOnlyCreateInfo = {};
		allocGpuOnlyCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocGpuOnlyCreateInfo.flags = 0;
		vmaCreateImage(m_allocator, &imageInfo, &allocGpuOnlyCreateInfo, &outImage, &outAlloc, nullptr);
	}

	void Allocator::CopyDataToImage(const VkExtent3D& extent, const VkFormat format, const uint32_t mipLevels, const void* data, VkImage& image, VmaAllocation& alloc)
	{
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
		memcpy(allocStagingBufferInfo.pMappedData, data, size);

		//  Copy image from staging buffer to image		
		TransitionImageLayout(image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);

		CopyBufferToImage(extent, stageBuffer, image);

		vmaDestroyBuffer(m_allocator, stageBuffer, stageBufferAlloc);
	}

	void Allocator::CreateImageView(const VkImage image, const VkFormat format, const uint32_t mipLevels, const VkImageAspectFlags aspectFlags, VkImageView& outImagaView)
	{
		VkImageViewCreateInfo imageViewCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		imageViewCreateInfo.image = image;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = format;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
		imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = mipLevels;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		VKERRCHECK(vkCreateImageView(m_device, &imageViewCreateInfo, nullptr, &outImagaView));
	}

	void Allocator::GenerateMipmaps(const VkImage image, const VkFormat imageFormat, const int32_t texWidth, const int32_t texHeight, const uint32_t mipLevels)
	{
		// Check if image format supports linear blitting
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(m_gpu, imageFormat, &formatProperties);

		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
		{
			LOGE("Texture image format does not support linear blitting!");
			exit(0);
		}

		BeginCommandBuffer();

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

		EndCommandBuffer();
	}

	void Allocator::TransitionImageLayout(const VkImage image, const VkImageLayout oldLayout, const VkImageLayout newLayout, const uint32_t mipLevels)
	{
		BeginCommandBuffer();

		VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;  // NOTE: subresourceRange is same as the one in ImageView
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mipLevels;
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

		EndCommandBuffer();
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
		: m_allocator(allocator), m_allocation(), m_buffer(), m_count(), m_stride()
	{
	}

	Buffer::~Buffer()
	{
		Clear();
	}

	void Buffer::Clear()
	{
		vkQueueWaitIdle(m_allocator.GetQueue());

		if (m_buffer)
		{
			m_allocator.DestroyBuffer(m_buffer, m_allocation);
		}

		m_buffer = 0;
		m_count = 0;
		m_stride = 0;
	}

	void Buffer::Data(const void* data, const uint32_t count, const uint32_t stride, const VkBufferUsageFlagBits usage, const VmaMemoryUsage memtype, void** mapped)
	{
		Clear();

		m_allocator.CreateBuffer(data, count * stride, usage, memtype, m_buffer, m_allocation, mapped);

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


	void VBO::Data(const void* data, const uint32_t count, const uint32_t stride)
	{
		Buffer::Data(data, count, stride, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
	}
	//----------------------------------------------------

	//--------------------------IBO-----------------------
	void IBO::Data(const uint16_t* data, const uint32_t count)
	{
		Buffer::Data(data, count, 2, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
	}

	void IBO::Data(const uint32_t* data, const uint32_t count)
	{
		Buffer::Data(data, count, 4, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
	}



	void UBO::Allocate(const uint32_t size)
	{
		Buffer::Data(0, 1, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, &m_mapped);
	}

	void UBO::Update(const void* data)
	{
		memcpy(m_mapped, data, m_stride);
	}

	uint32_t UBO::GetSize() const
	{
		return m_stride;
	}


	//--------------------------------------------------------------------------------
	AbstractImageBuffer::AbstractImageBuffer(Allocator& allocator)
		: m_allocator(allocator), m_allocation(), m_image(), m_imageView(), m_sampler(), m_extent(), m_format()
	{
	}

	AbstractImageBuffer::~AbstractImageBuffer()
	{
		Destroy();
	}

	void AbstractImageBuffer::CreateSampler(const float maxLod)
	{
		if (m_sampler)
		{
			vkDestroySampler(m_allocator.GetDevice(), m_sampler, nullptr);
		}

		VkSamplerCreateInfo samplerInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.mipLodBias = 0;

		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = 16;
		//samplerInfo.anisotropyEnable = VK_FALSE;
		//samplerInfo.maxAnisotropy = 1;

		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.minLod = 0;
		samplerInfo.maxLod = maxLod;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		VKERRCHECK(vkCreateSampler(m_allocator.GetDevice(), &samplerInfo, nullptr, &m_sampler));
	}

	void AbstractImageBuffer::UpdateSampler(const VkSamplerCreateInfo& samplerInfo)
	{
		if (m_sampler)
		{
			vkDestroySampler(m_allocator.GetDevice(), m_sampler, nullptr);
		}

		VKERRCHECK(vkCreateSampler(m_allocator.GetDevice(), &samplerInfo, nullptr, &m_sampler));
	}

	void AbstractImageBuffer::Destroy()
	{
		vkQueueWaitIdle(m_allocator.GetQueue());

		if (m_sampler)
		{
			vkDestroySampler(m_allocator.GetDevice(), m_sampler, nullptr);
		}

		if (m_image)
		{
			m_allocator.DestroyImage(m_image, m_imageView, m_allocation);
		}

		m_image = VK_NULL_HANDLE;
		m_sampler = VK_NULL_HANDLE;
		m_extent = {};
	}

	VkImage AbstractImageBuffer::GetImage() const
	{
		return m_image;
	}

	VkImageView AbstractImageBuffer::GetImageView() const
	{
		return m_imageView;
	}

	VkSampler AbstractImageBuffer::GetSampler() const
	{
		return m_sampler;
	}

	VkFormat AbstractImageBuffer::GetFormat() const
	{
		return m_format;
	}

	VkExtent2D AbstractImageBuffer::GetExtent() const
	{
		return m_extent;
	}

	bool AbstractImageBuffer::HasMipMaps() const
	{
		return m_mipMaps;
	}


	//--------------------------------------------------------------------------------
	ImageBuffer::ImageBuffer(Allocator& allocator)
		: AbstractImageBuffer(allocator)
	{
	}

	ImageBuffer::~ImageBuffer()
	{
	}

	void ImageBuffer::Create(const ImageBufferCreateInfo& createInfo)
	{
		m_mipMaps = createInfo.mipMap;
		m_format = createInfo.format;

		uint32_t mipLevels = 1;
		if (createInfo.mipMap)
		{
			mipLevels = Log2(std::max(createInfo.extent.width, createInfo.extent.height)) + 1;
		}

		VkExtent3D ext3D{ createInfo.extent.width, createInfo.extent.height, 1 };

		m_allocator.CreateImage(ext3D, createInfo.format, mipLevels, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, m_image, m_allocation);
		m_allocator.CopyDataToImage(ext3D, createInfo.format, mipLevels, createInfo.data, m_image, m_allocation);

		if (mipLevels > 1)
		{
			m_allocator.GenerateMipmaps(m_image, createInfo.format, createInfo.extent.width, createInfo.extent.height, mipLevels);
		}
		else
		{
			m_allocator.TransitionImageLayout(m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels);
		}

		m_allocator.CreateImageView(m_image, createInfo.format, mipLevels, VK_IMAGE_ASPECT_COLOR_BIT, m_imageView);

		CreateSampler((float)mipLevels);
	}

	void ImageBuffer::Resize(const VkExtent2D& extent)
	{
		LOGW("Texture can not be resized - it has fixed size");
	}


	//--------------------------------------------------------------------------------
	DepthImageBuffer::DepthImageBuffer(Allocator& allocator)
		: AbstractImageBuffer(allocator)
	{
	}

	DepthImageBuffer::~DepthImageBuffer()
	{
	}

	void DepthImageBuffer::Create(const ImageBufferCreateInfo& createInfo)
	{
		m_format = createInfo.format;
		m_mipMaps = createInfo.mipMap;
		m_sampler = VK_NULL_HANDLE;

		Resize(createInfo.extent);
	}

	void DepthImageBuffer::Resize(const VkExtent2D& extent)
	{
		Destroy();

		VkExtent3D ext3D{ extent.width, extent.height, 1 };

		m_allocator.CreateImage(ext3D, m_format, 1, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, m_image, m_allocation);
		m_allocator.CreateImageView(m_image, m_format, 1, VK_IMAGE_ASPECT_DEPTH_BIT, m_imageView);

		m_extent = extent;
	}


	//--------------------------------------------------------------------------------
	ColorImageBuffer::ColorImageBuffer(Allocator& allocator)
		: AbstractImageBuffer(allocator)
	{
	}

	ColorImageBuffer::~ColorImageBuffer()
	{
	}

	void ColorImageBuffer::Create(const ImageBufferCreateInfo& createInfo)
	{
		m_format = createInfo.format;
		m_mipMaps = createInfo.mipMap;
		m_sampler = VK_NULL_HANDLE;

		Resize(createInfo.extent);
	}

	void ColorImageBuffer::Resize(const VkExtent2D& extent)
	{
		Destroy();

		VkExtent3D ext3D{ extent.width, extent.height, 1 };

		m_allocator.CreateImage(ext3D, m_format, 1, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_image, m_allocation);
		m_allocator.CreateImageView(m_image, m_format, 1, VK_IMAGE_ASPECT_COLOR_BIT, m_imageView);

		m_extent = extent;
	}
}