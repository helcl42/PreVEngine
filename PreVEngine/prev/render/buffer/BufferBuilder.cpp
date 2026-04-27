#include "BufferBuilder.h"

#include "../../util/MathUtils.h"
#include "../../util/Utils.h"

#include <stdexcept>

namespace prev::render::buffer {
BufferBuilder::BufferBuilder(const prev::core::device::Device& device, const prev::core::device::Queue& queue)
    : m_device{ device }
    , m_queue{ queue }
{
}

BufferBuilder& BufferBuilder::SetUsageFlags(const GfxBufferUsageFlags usageFlags)
{
    m_usageFlags = usageFlags;
    return *this;
}

BufferBuilder& BufferBuilder::SetHostMapped(const bool hostMapped)
{
    m_hostMapped = hostMapped;
    return *this;
}

BufferBuilder& BufferBuilder::SetSize(const uint64_t size)
{
    m_size = size;
    return *this;
}

BufferBuilder& BufferBuilder::SetAlignment(const uint64_t alignment)
{
    m_alignment = alignment;
    return *this;
}

BufferBuilder& BufferBuilder::SetData(const void* data, const uint64_t size)
{
    m_data = data;
    m_dataSize = size;
    return *this;
}

std::unique_ptr<Buffer> BufferBuilder::Build() const
{
    Validate();

    const uint64_t alignedSize{ prev::util::math::RoundUp(m_size, m_alignment) };

    GfxBufferUsageFlags usageFlags{ m_usageFlags };
    GfxMemoryPropertyFlags memProps{};
    if (m_hostMapped) {
        memProps = GFX_MEMORY_PROPERTY_HOST_VISIBLE | GFX_MEMORY_PROPERTY_HOST_COHERENT;
        usageFlags |= GFX_BUFFER_USAGE_MAP_WRITE | GFX_BUFFER_USAGE_MAP_READ;
    } else {
        memProps = GFX_MEMORY_PROPERTY_DEVICE_LOCAL;
        usageFlags |= GFX_BUFFER_USAGE_COPY_DST;
    }

    GfxBufferDescriptor desc{};
    desc.sType = GFX_STRUCTURE_TYPE_BUFFER_DESCRIPTOR;
    desc.size = alignedSize;
    desc.usage = usageFlags;
    desc.memoryProperties = memProps;

    GfxBuffer buffer{};
    if (gfxDeviceCreateBuffer(m_device, &desc, &buffer) != GFX_RESULT_SUCCESS || !buffer) {
        throw std::runtime_error("Could not allocate buffer: size = " + std::to_string(alignedSize) + " bytes");
    }

    void* mappedPtr{};
    if (m_hostMapped) {
        gfxBufferMap(buffer, 0, GFX_WHOLE_SIZE, &mappedPtr);
    }

    if (mappedPtr) {
        const auto sizeToInit{ std::min(m_dataSize, alignedSize) };
        if (m_data) {
            std::memcpy(mappedPtr, m_data, sizeToInit);
        } else {
            std::memset(mappedPtr, 0, alignedSize);
        }
        gfxBufferFlushMappedRange(buffer, 0, alignedSize);
    } else if (m_data && m_dataSize > 0) {
        gfxQueueWriteBuffer(m_queue, buffer, 0, m_data, std::min(m_dataSize, alignedSize));
    }

    return std::unique_ptr<Buffer>(new Buffer(m_device, m_queue, buffer, m_hostMapped, alignedSize, mappedPtr));
}

void BufferBuilder::Validate() const
{
    if (m_usageFlags == 0) {
        throw std::runtime_error("Could not create buffer with usage flags 0");
    }

    if (m_size == 0) {
        throw std::runtime_error("Could not create buffer with size 0");
    }
}
} // namespace prev::render::buffer