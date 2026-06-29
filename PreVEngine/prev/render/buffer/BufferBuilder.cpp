#include "BufferBuilder.h"
#include "OwnedGfxBuffer.h"

#include "../../core/DeferredResourceDestroyer.h"
#include "../../core/DeferredResourceUploader.h"
#include "../../util/MathUtils.h"
#include "../../util/Utils.h"

#include <atomic>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <vector>

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

BufferBuilder& BufferBuilder::SetDestroyExecutionMode(ExecutionMode executionMode)
{
    m_destroyExecutionMode = executionMode;
    return *this;
}

std::unique_ptr<Buffer> BufferBuilder::Build() const
{
    return BuildImpl(nullptr);
}

std::unique_ptr<Buffer> BufferBuilder::Build(GfxCommandEncoder commandEncoder) const
{
    return BuildImpl(commandEncoder);
}

std::unique_ptr<Buffer> BufferBuilder::CreateBuffer(uint64_t& outAlignedSize, bool& outHostMapped, const std::shared_ptr<std::atomic<prev::core::ResourceState>>& stateFlag) const
{
    Validate();

    const uint64_t alignedSize{ prev::util::math::RoundUp(m_size, m_alignment) };

    GfxBufferDescriptor desc{};
    desc.sType = GFX_STRUCTURE_TYPE_BUFFER_DESCRIPTOR;
    desc.size = alignedSize;
    desc.usage = m_usageFlags;
    desc.memoryProperties = m_memoryProperties;

    GfxBuffer buffer{};
    if (gfxDeviceCreateBuffer(m_device, &desc, &buffer) != GFX_RESULT_SUCCESS || !buffer) {
        throw std::runtime_error("Could not allocate buffer: size = " + std::to_string(alignedSize) + " bytes");
    }

    const bool hostMapped = (m_memoryProperties & GFX_MEMORY_PROPERTY_HOST_VISIBLE) != 0;

    Buffer::CreateInfo createInfo{};
    createInfo.buffer = buffer;
    createInfo.hostMapped = hostMapped;
    createInfo.size = alignedSize;
    createInfo.deferredResourceDestroyer = &m_device.GetDeferredResourceDestroyer();
    createInfo.destroyExecutionMode = m_destroyExecutionMode;
    createInfo.stateFlag = stateFlag;

    outAlignedSize = alignedSize;
    outHostMapped = hostMapped;
    return std::unique_ptr<Buffer>(new Buffer(m_device, m_queue, createInfo));
}

std::unique_ptr<Buffer> BufferBuilder::BuildImpl(GfxCommandEncoder commandEncoder) const
{
    // A buffer with initial data, or a host-mapped buffer (zero-filled below), is usable on return
    // (Ready); a device-local buffer left without data is allocated but unpopulated (None).
    const bool hasData{ m_data && m_dataSize > 0 };
    const bool willBeHostMapped{ (m_memoryProperties & GFX_MEMORY_PROPERTY_HOST_VISIBLE) != 0 };
    auto state{ std::make_shared<std::atomic<prev::core::ResourceState>>((hasData || willBeHostMapped) ? prev::core::ResourceState::Ready : prev::core::ResourceState::None) };

    uint64_t alignedSize{};
    bool hostMapped{};
    auto buffer{ CreateBuffer(alignedSize, hostMapped, state) };

    const GfxBuffer raw{ *buffer };
    if (m_data && m_dataSize > 0) {
        UploadData(raw, std::min(m_dataSize, alignedSize), commandEncoder);
    } else if (hostMapped) {
        std::vector<uint8_t> zeros(alignedSize, 0);
        gfxQueueWriteBuffer(m_queue, raw, 0, zeros.data(), alignedSize);
    }

    return buffer;
}

std::unique_ptr<Buffer> BufferBuilder::BuildAsync() const
{
    if (!m_data || m_dataSize == 0) {
        // Nothing to stream; an async build with no data has no benefit, so build it ready immediately.
        return BuildImpl(nullptr);
    }

    if (!m_device.GetDeferredResourceUploader().CanQueue(m_dataSize)) {
        // Too much staging already queued (e.g. a whole scene at load); build synchronously so this data's
        // staging is freed immediately rather than held until flush, keeping peak memory bounded.
        return BuildImpl(nullptr);
    }

    // Allocate now but leave it Creating; the uploader records the copy at frame start and flips it Ready.
    // The shared state survives the resource being dropped before then (its destructor cancels the upload).
    auto state{ std::make_shared<std::atomic<prev::core::ResourceState>>(prev::core::ResourceState::Creating) };

    uint64_t alignedSize{};
    bool hostMapped{};
    auto buffer{ CreateBuffer(alignedSize, hostMapped, state) };

    const uint64_t size{ std::min(m_dataSize, alignedSize) };
    const GfxBuffer staging{ CreateStagingBuffer(size) };
    auto record{ MakeCopyRecorder(staging, *buffer, size) };

    m_device.GetDeferredResourceUploader().Enqueue(std::move(record), state, staging, size);

    return buffer;
}

void BufferBuilder::UploadData(GfxBuffer buffer, uint64_t size, GfxCommandEncoder commandEncoder) const
{
    if (commandEncoder) {
        // Record into the caller's encoder: stage the data and record a buffer->buffer copy. The copy runs
        // when the caller submits the encoder; the staging buffer is defer-destroyed so it outlives it.
        const GfxBuffer staging{ CreateStagingBuffer(size) };
        MakeCopyRecorder(staging, buffer, size)(commandEncoder);
        m_device.GetDeferredResourceDestroyer().Destroy(std::make_unique<OwnedGfxBuffer>(staging));
    } else {
        // Immediate: gfxQueueWriteBuffer handles host-visible (direct write) and device-local (internal
        // staging) targets, so the buffer holds its data when Build() returns.
        gfxQueueWriteBuffer(m_queue, buffer, 0, m_data, size);
    }
}

GfxBuffer BufferBuilder::CreateStagingBuffer(uint64_t size) const
{
    GfxBufferDescriptor stagingDesc{};
    stagingDesc.sType = GFX_STRUCTURE_TYPE_BUFFER_DESCRIPTOR;
    stagingDesc.size = size;
    stagingDesc.usage = GFX_BUFFER_USAGE_MAP_WRITE | GFX_BUFFER_USAGE_COPY_SRC;
    stagingDesc.memoryProperties = GFX_MEMORY_PROPERTY_HOST_VISIBLE | GFX_MEMORY_PROPERTY_HOST_COHERENT;

    GfxBuffer staging{};
    if (gfxDeviceCreateBuffer(m_device, &stagingDesc, &staging) != GFX_RESULT_SUCCESS) {
        throw std::runtime_error("Failed to create staging buffer for buffer upload");
    }

    void* mapped{ nullptr };
    if (gfxBufferMap(staging, 0, size, &mapped) != GFX_RESULT_SUCCESS) {
        gfxBufferDestroy(staging);
        throw std::runtime_error("Failed to map staging buffer for buffer upload");
    }
    memcpy(mapped, m_data, size);
    gfxBufferUnmap(staging);

    return staging;
}

std::function<void(GfxCommandEncoder)> BufferBuilder::MakeCopyRecorder(GfxBuffer staging, GfxBuffer destination, uint64_t size) const
{
    return [staging, destination, size](GfxCommandEncoder enc) {
        GfxCopyBufferToBufferDescriptor copyDesc{};
        copyDesc.source = staging;
        copyDesc.sourceOffset = 0;
        copyDesc.destination = destination;
        copyDesc.destinationOffset = 0;
        copyDesc.size = size;
        gfxCommandEncoderCopyBufferToBuffer(enc, &copyDesc);
    };
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