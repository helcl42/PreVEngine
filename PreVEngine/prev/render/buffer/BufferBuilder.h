#ifndef __BUFFER_BUILDER_H__
#define __BUFFER_BUILDER_H__

#include "Buffer.h"
#include "ExecutionMode.h"

#include "../../core/DeferredResourceDestroyer.h"
#include "../../core/DeferredResourceUploader.h"
#include "../../core/device/Device.h"
#include "../../core/device/Queue.h"

#include <atomic>
#include <functional>
#include <memory>

namespace prev::render::buffer {
class BufferBuilder final {
public:
    BufferBuilder(const prev::core::device::Device& device, const prev::core::device::Queue& queue);

    ~BufferBuilder() = default;

public:
    BufferBuilder& SetUsageFlags(const GfxBufferUsageFlags usageFlags);

    BufferBuilder& SetMemoryProperties(const GfxMemoryPropertyFlags memoryProperties);

    BufferBuilder& SetSize(const uint64_t size);

    BufferBuilder& SetAlignment(const uint64_t alignment);

    BufferBuilder& SetData(const void* data, const uint64_t size);

    BufferBuilder& SetDestroyExecutionMode(ExecutionMode executionMode);

    // Builds the buffer and uploads its data immediately (submit + wait); ready to use on return.
    std::unique_ptr<Buffer> Build() const;

    // Builds the buffer and records its data upload into the given command encoder instead of submitting
    // — no wait. Ready once the caller submits that encoder, so the caller MUST record this before the
    // work that reads the buffer. Staging is defer-destroyed via the resource manager.
    std::unique_ptr<Buffer> Build(GfxCommandEncoder commandEncoder) const;

    // Builds the buffer immediately but leaves it unpopulated and NOT ready: the data upload is queued
    // with the device's DeferredResourceUploader and recorded into a frame's encoder at frame start. The
    // buffer flips to ready once flushed. Consumers MUST skip it while IsReady() is false. The returned
    // buffer must be kept alive until it becomes ready. No queue stall.
    std::unique_ptr<Buffer> BuildAsync() const;

private:
    void Validate() const;

    // Shared implementation: commandEncoder == nullptr -> immediate (submit + wait); otherwise records
    // the upload into the encoder.
    std::unique_ptr<Buffer> BuildImpl(GfxCommandEncoder commandEncoder) const;

    // Allocates the GfxBuffer and constructs the (unpopulated) Buffer with the given initial/shared
    // lifecycle state (Ready/None for sync builds, a shared Creating state for async builds). Outputs the
    // aligned size and whether the buffer is host-mapped.
    std::unique_ptr<Buffer> CreateBuffer(uint64_t& outAlignedSize, bool& outHostMapped, const std::shared_ptr<std::atomic<prev::core::ResourceState>>& stateFlag) const;

    void UploadData(GfxBuffer buffer, uint64_t size, GfxCommandEncoder commandEncoder) const;

    // Creates a host-visible staging buffer filled with the builder's data. Caller owns it.
    GfxBuffer CreateStagingBuffer(uint64_t size) const;

    // Builds the staging->buffer copy recorder.
    std::function<void(GfxCommandEncoder)> MakeCopyRecorder(GfxBuffer staging, GfxBuffer destination, uint64_t size) const;

private:
    const prev::core::device::Device& m_device;

    const prev::core::device::Queue& m_queue;

    GfxBufferUsageFlags m_usageFlags{};

    GfxMemoryPropertyFlags m_memoryProperties{};

    uint64_t m_size{ 0 };

    uint64_t m_alignment{ 8 };

    const void* m_data{ nullptr };

    uint64_t m_dataSize{};

    ExecutionMode m_destroyExecutionMode{ ExecutionMode::Auto };
};
} // namespace prev::render::buffer

#endif // !__BUFFER_BUILDER_H__