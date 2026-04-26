#ifndef __BUFFER_POOL_H__
#define __BUFFER_POOL_H__

#include "Buffer.h"

#include "../../util/Utils.h"

namespace prev::render::buffer {
class BufferPoolBuilder;

class BufferPool final {
private:
    explicit BufferPool(std::vector<std::unique_ptr<Buffer>>&& buffers);

public:
    ~BufferPool() = default;

public:
    Buffer& GetCurrent() const;

    void MoveToNext();

public:
    friend class BufferPoolBuilder;

private:
    std::vector<std::unique_ptr<Buffer>> m_buffers;

    prev::util::CircularIndex<uint32_t> m_index{ 0 };
};
} // namespace prev::render::buffer

#endif // !__BUFFER_POOL_H__