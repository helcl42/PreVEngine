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
	private:
		VkIndexType m_indexType = VK_INDEX_TYPE_MAX_ENUM;

	public:
		using Buffer::Buffer;

	public:
		void Data(const uint16_t* data, const uint32_t count);

		void Data(const uint32_t* data, const uint32_t count);

		VkIndexType GetIndexType() const;
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
	struct ImageBufferCreateInfo
	{
		VkExtent2D extent;

		VkFormat format;

		bool mipMap;

		const void* data;

		ImageBufferCreateInfo(const VkExtent2D& ext, const VkFormat fmt, const bool mipmap = false, const void* imageData = nullptr)
			: extent(ext), format(fmt), mipMap(mipmap), data(imageData)
		{
		}

		~ImageBufferCreateInfo()
		{
		}
	};

	class IImageBuffer
	{
	public:
		virtual void Create(const ImageBufferCreateInfo& createInfo) = 0;

		virtual void Resize(const VkExtent2D& extent) = 0;

		virtual void Destroy() = 0;

		virtual void CreateSampler(const float maxLod = 1.0f) = 0;

		virtual void UpdateSampler(const VkSamplerCreateInfo& samplerInfo) = 0;

		virtual VkImage GetImage() const = 0;

		virtual VkImageView GetImageView() const = 0;

		virtual VkSampler GetSampler() const = 0;

		virtual VkFormat GetFormat() const = 0;

		virtual VkExtent2D GetExtent() const = 0;

		virtual bool HasMipMaps() const = 0;

	public:
		virtual ~IImageBuffer() {}
	};

	class AbstractImageBuffer : public IImageBuffer
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

	public:
		void UpdateSampler(const VkSamplerCreateInfo& samplerInfo) override;

		void Destroy() override;

		void CreateSampler(const float maxLod = 1.0f) override;

	public:
		VkImage GetImage() const override;

		VkImageView GetImageView() const override;

		VkSampler GetSampler() const override;

		VkFormat GetFormat() const override;

		VkExtent2D GetExtent() const override;

		bool HasMipMaps() const override;
	};

	//----------------------------------Image Buffer----------------------------------
	class ImageBuffer : public AbstractImageBuffer
	{
	public:
		ImageBuffer(Allocator& allocator);

		~ImageBuffer();

	public:
		void Create(const ImageBufferCreateInfo& createInfo) override;

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
		void Create(const ImageBufferCreateInfo& createInfo) override;

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
		void Create(const ImageBufferCreateInfo& createInfo) override;

		void Resize(const VkExtent2D& extent) override;
	};
	//--------------------------------------------------------------------------------
}

#endif