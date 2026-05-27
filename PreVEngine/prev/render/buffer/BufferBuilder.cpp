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

BufferBuilder& BufferBuilder::SetMemoryProperties(const GfxMemoryPropertyFlags memoryProperties)
{
    m_memoryProperties = memoryProperties;
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
    GfxMemoryPropertyFlags memProps{ m_memoryProperties };

    GfxBufferDescriptor desc{};
    desc.sType = GFX_STRUCTURE_TYPE_BUFFER_DESCRIPTOR;
    desc.size = alignedSize;
    desc.usage = usageFlags;
    desc.memoryProperties = memProps;

    GfxBuffer buffer{};
    if (gfxDeviceCreateBuffer(m_device, &desc, &buffer) != GFX_RESULT_SUCCESS || !buffer) {
        throw std::runtime_error("Could not allocate buffer: size = " + std::to_string(alignedSize) + " bytes");
    }

    if (m_data && m_dataSize > 0) {
        gfxQueueWriteBuffer(m_queue, buffer, 0, m_data, std::min(m_dataSize, alignedSize));
    } else if (memProps & GFX_MEMORY_PROPERTY_HOST_VISIBLE) {
        std::vector<uint8_t> zeros(alignedSize, 0);
        gfxQueueWriteBuffer(m_queue, buffer, 0, zeros.data(), alignedSize);
    }

    const bool hostMapped = (memProps & GFX_MEMORY_PROPERTY_HOST_VISIBLE) != 0;
    return std::unique_ptr<Buffer>(new Buffer(m_device, m_queue, buffer, hostMapped, alignedSize));
}

void BufferBuilder::Validate() const
{
    if (m_usageFlags == 0) {
        throw std::runtime_error("Could not create buffer with usage flags 0");
    }

    if (m_memoryProperties == 0) {
        throw std::runtime_error("Could not create buffer with memory properties 0");
    }

    if (m_size == 0) {
        throw std::runtime_error("Could not create buffer with size 0");
    }
}
} // namespace prev::render::buffer