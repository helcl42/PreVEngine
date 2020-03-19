#ifndef __BUFFERS_H__
#define __BUFFERS_H__

#include "Common.h"
#include "Patterns.h"
#include "Devices.h"
#include "Utils.h"

#include "External/vk_mem_alloc.h"

namespace PreVEngine {
//------------------------------------Allocator-----------------------------------
class Allocator {
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
    void TransitionImageLayout(const VkImage image, const VkImageLayout oldLayout, const VkImageLayout newLayout, const uint32_t mipLevels = 1, const uint32_t layersCount = 1);

    void CreateBuffer(const void* data, uint64_t size, VkBufferUsageFlags usage, VmaMemoryUsage memtype, VkBuffer& buffer, VmaAllocation& alloc, void** mapped = 0);

    void DestroyBuffer(VkBuffer buffer, VmaAllocation alloc);

    void CopyBuffer(const VkBuffer srcBuffer, const VkDeviceSize size, VkBuffer dstBuffer);

    void CopyBufferToImage(const VkExtent3D& extent, const VkBuffer buffer, const uint32_t layerIndex, VkImage image);

    void CreateImage(const VkExtent3D& extent, const VkImageType imageType, const VkFormat format, const uint32_t mipLevels, const uint32_t layerCount, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkImageCreateFlags flags, VkImage& outImage, VmaAllocation& outAlloc);

    void CopyDataToImage(const VkExtent3D& extent, const VkFormat format, const uint32_t mipLevels, const std::vector<const uint8_t*> layerData, const uint32_t layerCount, VkImage& image);

    void CreateImageView(const VkImage image, const VkFormat format, const VkImageViewType viewType, const uint32_t mipLevels, const uint32_t layerCount, const VkImageAspectFlags aspectFlags, VkImageView& outImagaView);

    void DestroyImage(VkImage image, VkImageView view, VmaAllocation alloc);

    void GenerateMipmaps(const VkImage image, const VkFormat imageFormat, const int32_t texWidth, const int32_t texHeight, const uint32_t mipLevels, const uint32_t layersCount = 1);

public:
    VkPhysicalDevice GetPhysicalDevice() const;

    VkDevice GetDevice() const;

    VkQueue GetQueue() const;
};
//--------------------------------------------------------------------------------

//-------------------------------------Buffers------------------------------------
class Buffer {
protected:
    Allocator& m_allocator;

    VmaAllocation m_allocation;

    VkBuffer m_buffer;

    uint32_t m_count;

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

class VBO : public Buffer {
public:
    using Buffer::Buffer;

public:
    void Data(const void* data, const uint32_t count, const uint32_t stride);
};

class IBO : public Buffer {
private:
    VkIndexType m_indexType = VK_INDEX_TYPE_MAX_ENUM;

public:
    using Buffer::Buffer;

public:
    void Data(const uint16_t* data, const uint32_t count);

    void Data(const uint32_t* data, const uint32_t count);

    VkIndexType GetIndexType() const;
};

class UBO {
private:
    VkBuffer m_buffer;

    void* m_mapped;

    uint32_t m_offset;

    uint32_t m_range;

public:
    UBO(VkBuffer buffer, void* data, const uint32_t offset, const uint32_t range)
        : m_buffer(buffer)
        , m_mapped(data)
        , m_offset(offset)
        , m_range(range)
    {
    }

    ~UBO() = default;

public:
    void Update(const void* data)
    {
        memcpy(static_cast<uint8_t*>(m_mapped) + m_offset, data, m_range);
    }

    uint32_t GetOffset() const
    {
        return m_offset;
    }

    uint32_t GetRange() const
    {
        return m_range;
    }

    operator VkBuffer() const
    {
        return m_buffer;
    }
};

template <typename ItemType>
class UBOPool : public Buffer {
private:    
    std::vector<std::shared_ptr<UBO>> m_poolItems;

    uint32_t m_capacity;

    uint32_t m_index;

    void* m_mapped;

public:
    UBOPool(Allocator& allocator)
        : Buffer(allocator)
        , m_index(0)
        , m_capacity(0)
        , m_mapped(nullptr)
    {
    }

    virtual ~UBOPool() = default;

public:
    void AdjustCapactity(const uint32_t capacity, const uint32_t alignment = 32)
    {
        Clear();

        m_poolItems.clear();

        m_capacity = capacity;
        m_index = 0;
        
        const uint32_t itemSize = MathUtil::RoundUp(static_cast<uint32_t>(sizeof(ItemType)), alignment);

        Data(nullptr, capacity, itemSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, &m_mapped);
        
        for (uint32_t i = 0; i < capacity; i++) {
            auto ubo = std::make_shared<UBO>(m_buffer, m_mapped, i * itemSize, itemSize);
            m_poolItems.emplace_back(ubo);
        }
    }

    std::shared_ptr<UBO> GetNext()
    {
        m_index = (m_index + 1) % m_poolItems.size();
        return m_poolItems.at(m_index);
    }
};

//--------------------------------------------------------------------------------

//-------------------------------------AbstractImageBuffer-------------------------------------
struct ImageBufferCreateInfo {
    const VkExtent2D extent;

    const VkImageType imageType;

    const VkFormat format;

    const VkImageCreateFlags flags;

    const bool mipMap;

    const VkImageViewType viewType;

    const uint32_t layerCount;

    const VkSamplerAddressMode addressMode;

    const std::vector<const uint8_t*> layerData;

    ImageBufferCreateInfo(const VkExtent2D& ext, const VkImageType imgType, const VkFormat fmt, const VkImageCreateFlags flgs = 0, const bool mipmap = false, const VkImageViewType vwType = VK_IMAGE_VIEW_TYPE_2D, const uint32_t lrCount = 1, const VkSamplerAddressMode mode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, const uint8_t* data = nullptr)
        : ImageBufferCreateInfo(ext, imgType, fmt, flgs, mipmap, vwType, lrCount, mode, std::vector<const uint8_t*>{ data })
    {
    }

    ImageBufferCreateInfo(const VkExtent2D& ext, const VkImageType imgType, const VkFormat fmt, const VkImageCreateFlags flgs, const bool mipmap, const VkImageViewType vwType, const uint32_t lrCount, const VkSamplerAddressMode mode, const std::vector<const uint8_t*>& lrImageData)
        : extent(ext)
        , imageType(imgType)
        , format(fmt)
        , flags(flgs)
        , mipMap(mipmap)
        , viewType(vwType)
        , layerCount(lrCount)
        , addressMode(mode)
        , layerData(lrImageData)
    {
    }

    ~ImageBufferCreateInfo() = default;
};

class IImageBuffer {
public:
    virtual void Create(const ImageBufferCreateInfo& createInfo) = 0;

    virtual void Resize(const VkExtent2D& extent) = 0;

    virtual void Destroy() = 0;

    virtual void CreateSampler(const float maxLod = 1.0f, const VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE) = 0;

    virtual void UpdateSampler(const VkSamplerCreateInfo& samplerInfo) = 0;

    virtual VkImage GetImage() const = 0;

    virtual VkImageView GetImageView() const = 0;

    virtual VkSampler GetSampler() const = 0;

    virtual VkFormat GetFormat() const = 0;

    virtual VkExtent2D GetExtent() const = 0;

    virtual VkImageType GetImageType() const = 0;

    virtual VkImageCreateFlags GetFlags() const = 0;

    virtual uint32_t GetMipLevels() const = 0;

    virtual uint32_t GetLayerCount() const = 0;

    virtual VkImageViewType GetViewType() const = 0;

public:
    virtual ~IImageBuffer() = default;
};

class AbstractImageBuffer : public IImageBuffer {
protected:
    Allocator& m_allocator;

    VmaAllocation m_allocation;

    VkImage m_image;

    VkExtent2D m_extent;

    VkImageType m_imageType;

    VkImageCreateFlags m_flags;

    VkFormat m_format;

    VkImageView m_imageView;

    VkSampler m_sampler;

    uint32_t m_mipLevels;

    uint32_t m_layerCount;

    VkImageViewType m_imageViewType;

public:
    AbstractImageBuffer(Allocator& allocator);

    virtual ~AbstractImageBuffer();

public:
    void UpdateSampler(const VkSamplerCreateInfo& samplerInfo) override;

    void Destroy() override;

    void CreateSampler(const float maxLod = 1.0f, const VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE) override;

public:
    VkImage GetImage() const override;

    VkImageView GetImageView() const override;

    VkSampler GetSampler() const override;

    VkFormat GetFormat() const override;

    VkExtent2D GetExtent() const override;

    VkImageType GetImageType() const override;

    VkImageCreateFlags GetFlags() const override;

    uint32_t GetMipLevels() const override;

    uint32_t GetLayerCount() const override;

    VkImageViewType GetViewType() const override;
};

//----------------------------------Image Buffer----------------------------------
class ImageBuffer : public AbstractImageBuffer {
public:
    ImageBuffer(Allocator& allocator);

    ~ImageBuffer() = default;

public:
    void Create(const ImageBufferCreateInfo& createInfo) override;

    void Resize(const VkExtent2D& extent) override;
};
//--------------------------------------------------------------------------------

//----------------------------------Depth Buffer----------------------------------
class DepthImageBuffer : public AbstractImageBuffer {
public:
    DepthImageBuffer(Allocator& allocator);

    ~DepthImageBuffer() = default;

public:
    void Create(const ImageBufferCreateInfo& createInfo) override;

    void Resize(const VkExtent2D& extent) override;
};
//--------------------------------------------------------------------------------

//----------------------------------Color Buffer----------------------------------
class ColorImageBuffer : public AbstractImageBuffer {
public:
    ColorImageBuffer(Allocator& allocator);

    ~ColorImageBuffer() = default;

public:
    void Create(const ImageBufferCreateInfo& createInfo) override;

    void Resize(const VkExtent2D& extent) override;
};
//--------------------------------------------------------------------------------
} // namespace PreVEngine

#endif