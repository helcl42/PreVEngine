#ifndef __BUFFERS_H__
#define __BUFFERS_H__

#include "Window.h"
#include "Devices.h"

#include "External/vk_mem_alloc.h"

namespace PreVEngine
{
	//----------------------------------Depth Buffer----------------------------------
	class DepthBuffer
	{
	private:
		VkPhysicalDevice m_gpu;

		VkDevice m_device;

		VkFormat m_format;

		VkImage m_image;

		VkDeviceMemory m_imageMemory;

		VkImageView m_imageView;

	public:
		DepthBuffer();

		virtual ~DepthBuffer();

	public:
		void Create(VkPhysicalDevice gpu, VkDevice device, VkExtent2D extent, VkFormat format = VK_FORMAT_D32_SFLOAT);

		void Destroy();

		void Resize(VkExtent2D extent);

	public:
		VkFormat GetFormat() const;

		VkImage GetImage() const;

		VkImageView GetImageView() const;
	};
	//--------------------------------------------------------------------------------

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
		Allocator(const Queue& queue, VkDeviceSize blockSize = 256);

		virtual ~Allocator();

	public:
		void BeginCmd();

		void EndCmd();

		void TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels = 1);

		void CreateBuffer(const void* data, uint64_t size, VkBufferUsageFlags usage, VmaMemoryUsage memtype, VkBuffer& buffer, VmaAllocation& alloc, void** mapped = 0);

		void DestroyBuffer(VkBuffer buffer, VmaAllocation alloc);

		void CreateImage(const void* data, VkExtent3D extent, VkFormat format, uint32_t mipLevels, VkImage& image, VmaAllocation& alloc, VkImageView& view);

		void DestroyImage(VkImage image, VkImageView view, VmaAllocation alloc);

		void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

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
		Allocator* m_allocator;

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

		void Data(const void* data, uint32_t count, uint32_t stride, VkBufferUsageFlagBits usage, VmaMemoryUsage memtype = VMA_MEMORY_USAGE_GPU_ONLY, void** mapped = nullptr);

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
		void Data(void* data, uint32_t count, uint32_t stride);
	};

	class IBO : public Buffer
	{
	public:
		using Buffer::Buffer;

	public:
		void Data(const uint16_t* data, uint32_t count);

		void Data(const uint32_t* data, uint32_t count);
	};

	class UBO : public Buffer
	{
	private:
		void* m_mapped = nullptr;

	public:
		using Buffer::Buffer;

	public:
		//void Data(void* data, uint32_t size);

		void Allocate(uint32_t size);

		void Update(void* data);

	public:
		uint32_t GetSize() const;
	};
	//--------------------------------------------------------------------------------

	//-------------------------------------Images-------------------------------------
	class ImageBuffer
	{
	private:
		Allocator* m_allocator;

		VmaAllocation m_allocation;

		VkImage m_image;

		VkExtent2D m_extent;

		VkFormat m_format;

		VkImageView m_imageView;

		VkSampler m_sampler;

	public:
		ImageBuffer(Allocator& allocator);

		virtual ~ImageBuffer();

	private:
		void CreateSampler(float maxLod = 0);

	public:
		void Clear();

		void Data(const void* data, VkExtent3D extent, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM, bool mipmap = false);

		void UpdateSampler(VkSamplerCreateInfo& samplerInfo); // Update sampler settings

	public:
		VkImage GetImage() const;

		VkImageView GetImageView() const;

		VkSampler GetSampler() const;

		VkFormat GetFormat() const;

		VkExtent2D GetExtent() const;

	public:
		//operator VkImage() { return image; }

		//operator VkImageView() { return view; }

		//operator VkSampler() { return sampler; }
	};
	//--------------------------------------------------------------------------------
}

#endif