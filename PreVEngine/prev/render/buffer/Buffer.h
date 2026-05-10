#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "../../core/device/Device.h"

#include <stdexcept>
#include <string>

namespace prev::render::buffer {
class BufferBuilder;
class BufferPoolBuilder;

class Buffer final {
private:
    Buffer(GfxDevice device, GfxQueue queue, GfxBuffer buffer, bool hostMapped, uint64_t size);

public:
    ~Buffer();

public:
    void Write(const void* data, const uint64_t size, const uint64_t offset = 0);

    void Clear();

public:
    uint64_t GetSize() const;

    template <typename T>
    void Write(const T& data)
    {
        Write(&data, sizeof(T), 0);
    }

public:
    operator GfxBuffer() const;

public:
    friend class BufferBuilder;
    friend class BufferPoolBuilder;

protected:
    GfxDevice m_device;

    GfxQueue m_queue;

    GfxBuffer m_buffer;

    bool m_hostMapped;

    uint64_t m_size;
};
} // namespace prev::render::buffer

#endif // !__BUFFER_H__
