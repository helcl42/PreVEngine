#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "../../core/device/Device.h"

#include <stdexcept>
#include <string>

namespace prev::render::buffer {
class BufferBuilder;

class Buffer final {
private:
    Buffer(GfxDevice device, GfxQueue queue, GfxBuffer buffer, bool hostMapped, uint64_t size, uint64_t offset = 0, bool owning = true);

public:
    ~Buffer();

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer(Buffer&&) noexcept;
    Buffer& operator=(Buffer&&) noexcept;

public:
    void Write(const void* data, const uint64_t size, const uint64_t offset = 0);

    void Clear();

public:
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

protected:
    GfxDevice m_device{};

    GfxQueue m_queue{};

    GfxBuffer m_buffer{};

    bool m_hostMapped{};

    uint64_t m_size{};

    uint64_t m_offset{};

    bool m_owning{ true };
};
} // namespace prev::render::buffer

#endif // !__BUFFER_H__
