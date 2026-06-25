#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "ExecutionMode.h"

#include "../../core/DeferredResourceDestroyer.h"
#include "../../core/IResource.h"
#include "../../core/device/Device.h"

#include <atomic>
#include <memory>
#include <stdexcept>
#include <string>

namespace prev::render::buffer {
class BufferBuilder;

class Buffer final : public prev::core::IResource {
private:
    struct CreateInfo {
        GfxBuffer buffer{};
        bool hostMapped{};
        uint64_t size{};
        uint64_t offset{};
        bool owning{ true };
        prev::core::DeferredResourceDestroyer* deferredResourceDestroyer{};
        ExecutionMode destroyExecutionMode{ ExecutionMode::Auto };
        // Initial/shared lifecycle state. Null defaults to Ready; async builds share a Creating state
        // with the uploader so it survives the resource being dropped before the upload is flushed.
        std::shared_ptr<std::atomic<prev::core::ResourceState>> stateFlag{};
    };

    Buffer(GfxDevice device, GfxQueue queue, const CreateInfo& createInfo);

public:
    ~Buffer() override;

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer(Buffer&&) noexcept;
    Buffer& operator=(Buffer&&) noexcept;

public:
    void Write(const void* data, const uint64_t size, const uint64_t offset = 0);

    void Clear();

public:
    prev::core::ResourceState GetState() const override;

    uint64_t GetSize() const;

    uint64_t GetOffset() const;

    // Returns a non-owning view into this buffer at [offset, offset+size), sharing the backing
    // GfxBuffer. The view never frees the GPU buffer (the owning Buffer does), so it is safe to
    // outlive nothing beyond its parent. Used to carve a backing buffer into pool slices.
    Buffer Slice(uint64_t offset, uint64_t size) const;

    template <typename T>
    void Write(const T& data)
    {
        Write(&data, sizeof(T), 0);
    }

public:
    operator GfxBuffer() const;

public:
    friend class BufferBuilder; // constructs the owning buffer

private:
    bool IsDeferred() const;

    void ReleaseBuffer();

protected:
    GfxDevice m_device{};

    GfxQueue m_queue{};

    GfxBuffer m_buffer{};

    bool m_hostMapped{};

    uint64_t m_size{};

    uint64_t m_offset{};

    bool m_owning{ true };

    prev::core::DeferredResourceDestroyer* m_deferredResourceDestroyer{ nullptr };

    ExecutionMode m_destroyExecutionMode{ ExecutionMode::Auto };

    std::shared_ptr<std::atomic<prev::core::ResourceState>> m_state{};
};
} // namespace prev::render::buffer

#endif // !__BUFFER_H__
