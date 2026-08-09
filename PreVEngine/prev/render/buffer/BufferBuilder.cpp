#include "BufferBuilder.h"

#include "../../common/Common.h"
#include "../../core/DeferredResourceDestroyer.h"
#include "../../core/DeferredResourceUploader.h"
#include "../../core/OwnedGfxHandle.h"
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
    auto staged{ CreateStagedData(size) };
    auto record{ MakeCopyRecorder(staged.buffer, *buffer, size) };

    m_device.GetDeferredResourceUploader().Enqueue(std::move(record), state, std::move(staged));

    return buffer;
}

void BufferBuilder::UploadData(GfxBuffer buffer, uint64_t size, GfxCommandEncoder commandEncoder) const
{
    if (commandEncoder) {
        // Stage + record a copy into the caller's encoder; defer-destroy the staging so it outlives the submit.
        auto staged{ CreateStagedData(size) };
        if (staged.prepare) {
            // The platform could not map the staging synchronously; a queue write covers this path too.
            gfxBufferDestroy(staged.buffer);
            gfxQueueWriteBuffer(m_queue, buffer, 0, m_data, size);
            return;
        }
        MakeCopyRecorder(staged.buffer, buffer, size)(commandEncoder);
        m_device.GetDeferredResourceDestroyer().Destroy(std::make_unique<prev::core::OwnedGfxBuffer>(staged.buffer));
    } else {
        // Immediate: gfxQueueWriteBuffer covers host-visible and device-local targets.
        gfxQueueWriteBuffer(m_queue, buffer, 0, m_data, size);
    }
}

prev::core::DeferredResourceUploader::StagingData BufferBuilder::CreateStagedData(uint64_t size) const
{
    GfxBufferDescriptor stagingDesc{};
    stagingDesc.sType = GFX_STRUCTURE_TYPE_BUFFER_DESCRIPTOR;
    stagingDesc.size = size;
    stagingDesc.usage = GFX_BUFFER_USAGE_MAP_WRITE | GFX_BUFFER_USAGE_COPY_SRC;
    stagingDesc.memoryProperties = GFX_MEMORY_PROPERTY_HOST_VISIBLE | GFX_MEMORY_PROPERTY_HOST_COHERENT;

    prev::core::DeferredResourceUploader::StagingData staged{};
    staged.bytes = size;
    if (gfxDeviceCreateBuffer(m_device, &stagingDesc, &staged.buffer) != GFX_RESULT_SUCCESS) {
        throw std::runtime_error("Failed to create staging buffer for buffer upload");
    }
    // When the map does not land inline (web), the fill keeps its own copy and retries at flush.
    void* pointer{ nullptr };
    const GfxResult mapResult{ gfxBufferMapAsync(staged.buffer, 0, size, &pointer) };
    if (mapResult != GFX_RESULT_NOT_READY) {
        GFXERRCHECK(mapResult);
        memcpy(pointer, m_data, size);
        gfxBufferUnmap(staged.buffer);
        return staged;
    }
    staged.prepare = [staging = staged.buffer, data = std::vector<uint8_t>(static_cast<const uint8_t*>(m_data), static_cast<const uint8_t*>(m_data) + size)]() {
        void* pointer{ nullptr };
        if (gfxBufferMapAsync(staging, 0, data.size(), &pointer) != GFX_RESULT_SUCCESS) {
            return false;
        }
        memcpy(pointer, data.data(), data.size());
        gfxBufferUnmap(staging);
        return true;
    };
    return staged;
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