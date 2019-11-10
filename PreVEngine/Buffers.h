#ifndef __BUFFERS_H__
#define __BUFFERS_H__

#include "Window.h"
#include "Devices.h"

#include "External/vk_mem_alloc.h"

namespace PreVEngine
{
	//------------------------------------Allocator-----------------------------------
	class Allocator
	{
	private:
		VmaAllocator m_allocator;

		VkPhysicalDevice m_gpu;

		VkDevice m_device;

		VkQueue m_queue;

		VkCommandPool m_commandPool;

		VkCommandBuffer m_commandBuffer;

	public:
		Allocator(const Queue& queue, const VkDeviceSize blockSize = 256);

		virtual ~Allocator();

	public:
		void BeginCommandBuffer();

		void EndCommandBuffer();

	public:
		void TransitionImageLayout(const VkImage image, const VkImageLayout oldLayout, const VkImageLayout newLayout, const uint32_t mipLevels = 1);

		void CreateBuffer(const void* data, uint64_t size, VkBufferUsageFlags usage, VmaMemoryUsage memtype, VkBuffer& buffer, VmaAllocation& alloc, void** mapped = 0);

		void DestroyBuffer(VkBuffer buffer, VmaAllocation alloc);

		void CopyBuffer(const VkBuffer srcBuffer, const VkDeviceSize size, VkBuffer dstBuffer);

		void CopyBufferToImage(const VkExtent3D& extent, const VkBuffer buffer, VkImage image);

		void CreateImage(const VkExtent3D& extent, const VkFormat format, const uint32_t mipLevels, const VkImageTiling tiling, const VkImageUsageFlags usage, VkImage& outImage, VmaAllocation& outAlloc);

		void CopyDataToImage(const VkExtent3D& extent, const VkFormat format, const uint32_t mipLevels, const void* data, VkImage& image, VmaAllocation& alloc);

		void CreateImageView(const VkImage image, const VkFormat format, const uint32_t mipLevels, const VkImageAspectFlags aspectFlags, VkImageView& outImagaView);

		void DestroyImage(VkImage image, VkImageView view, VmaAllocation alloc);

		void GenerateMipmaps(const VkImage image, const VkFormat imageFormat, const int32_t texWidth, const int32_t texHeight, const uint32_t mipLevels);

	public:
		VkPhysicalDevice GetPhysicalDevice() const;

		VkDevice GetDevice() const;

		VkQueue GetQueue() const;
	};
	//--------------------------------------------------------------------------------

	//-------------------------------------Buffers------------------------------------
	class Buffer
	{
	private:
		Allocator& m_allocator;
		
		VmaAllocation m_allocation;

		VkBuffer m_buffer;

		uint32_t m_count;

	protected:
		uint32_t m_stride;

	public:
		Buffer(Allocator& allocator);

		virtual ~Buffer();

	public:
		void Clear();

		void Data(const void* data, const uint32_t count, const uint32_t stride, const VkBufferUsageFlagBits usage, const VmaMemoryUsage memtype = VMA_MEMORY_USAGE_GPU_ONLY, void** mapped = nullptr);

	public:
		uint32_t GetCount() const;

	public:
		operator VkBuffer() const;
	};

	class VBO : public Buffer
	{
	public:
		using Buffer::Buffer;

	public:
		void Data(const void* data, const uint32_t count, const uint32_t stride);
	};

	class IBO : public Buffer
	{
	public:
		using Buffer::Buffer;

	public:
		void Data(const uint16_t* data, const uint32_t count);

		void Data(const uint32_t* data, const uint32_t count);
	};

	class UBO : public Buffer
	{
	private:
		void* m_mapped = nullptr;

	public:
		using Buffer::Buffer;

	public:
		void Allocate(const uint32_t size);

		void Update(const void* data);

	public:
		uint32_t GetSize() const;
	};
	//--------------------------------------------------------------------------------

	//-------------------------------------AbstractImageBuffer-------------------------------------
	class AbstractImageBuffer
	{
	protected:
		Allocator& m_allocator;

		VmaAllocation m_allocation;

		VkImage m_image;

		VkExtent2D m_extent;

		VkFormat m_format;

		VkImageView m_imageView;

		VkSampler m_sampler;

		bool m_mipMaps;

	public:
		AbstractImageBuffer(Allocator& allocator);

		virtual ~AbstractImageBuffer();

	protected:
		void CreateSampler(const float maxLod = 0);

	public:
		void UpdateSampler(const VkSamplerCreateInfo& samplerInfo); // Update sampler settings

		void Destroy();

	public:
		virtual void Create(const VkExtent2D& extent, const VkFormat format) = 0;

		virtual void Resize(const VkExtent2D& extent) = 0;

	public:
		VkImage GetImage() const;

		VkImageView GetImageView() const;

		VkSampler GetSampler() const;

		VkFormat GetFormat() const;

		VkExtent2D GetExtent() const;

		bool HasMipMaps() const;
	};

	//----------------------------------Image Buffer----------------------------------
	class ImageBuffer : public AbstractImageBuffer
	{
	public:
		ImageBuffer(Allocator& allocator, const void* data, const VkExtent2D& extent, const VkFormat format = VK_FORMAT_R8G8B8A8_UNORM, const bool mipmap = false);

		~ImageBuffer();

	private:
		void Data(const void* data, const VkExtent2D& extent, const VkFormat format, const bool mipmap);

	public:
		void Create(const VkExtent2D& extent, const VkFormat format) override;

		void Resize(const VkExtent2D& extent) override;
	};
	//--------------------------------------------------------------------------------

	//----------------------------------Depth Buffer----------------------------------
	class DepthImageBuffer : public AbstractImageBuffer
	{
	public:
		DepthImageBuffer(Allocator& allocator);

		~DepthImageBuffer();

	public:
		void Create(const VkExtent2D& extent, const VkFormat format = VK_FORMAT_D32_SFLOAT) override;

		void Resize(const VkExtent2D& extent) override;
	};
	//--------------------------------------------------------------------------------

	//----------------------------------Color Buffer----------------------------------
	class ColorImageBuffer : public AbstractImageBuffer
	{
	public:
		ColorImageBuffer(Allocator& allocator);

		~ColorImageBuffer();

	public:
		void Create(const VkExtent2D& extent, const VkFormat format = VK_FORMAT_R8G8B8A8_UNORM) override;

		void Resize(const VkExtent2D& extent) override;
	};
	//--------------------------------------------------------------------------------
}

#endif