#ifndef __BUFFER_POOL_H__
#define __BUFFER_POOL_H__

#include "Buffer.h"

#include "../../util/Utils.h"

namespace prev::render::buffer {
class BufferPoolBuilder;

class BufferPool final {
private:
    explicit BufferPool(std::unique_ptr<Buffer> buffer, std::vector<Buffer>&& slices);

public:
    ~BufferPool() = default;

public:
    Buffer& GetCurrent();

    void MoveToNext();

public:
    friend class BufferPoolBuilder;

private:
    std::unique_ptr<Buffer> m_buffer;

    std::vector<Buffer> m_slices;

    prev::util::CircularIndex<uint32_t> m_index{ 0 };
};
} // namespace prev::render::buffer

#endif // !__BUFFER_POOL_H__