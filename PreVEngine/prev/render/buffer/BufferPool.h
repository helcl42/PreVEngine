#ifndef __BUFFER_POOL_H__
#define __BUFFER_POOL_H__

#include "Buffer.h"

#include "../../util/Utils.h"

#include <memory>
#include <vector>

namespace prev::render::buffer {
class BufferPoolBuilder;

// Fixed-size ring of buffer slices carved from one backing buffer. The caller must size it
// (SetCount) to comfortably exceed the number of slices live at once (e.g. draws/frame *
// frames-in-flight), or Next() wraps onto a slice still in flight. Suited to a fixed/known working
// set; for a per-frame set that grows with the scene, prefer FrameScopedBufferPool.
class BufferPool final {
private:
    explicit BufferPool(std::unique_ptr<Buffer> buffer, std::vector<Buffer>&& slices);

public:
    ~BufferPool() = default;

public:
    // Returns the current slice and advances the ring to the next one.
    Buffer& Next();

public:
    friend class BufferPoolBuilder;

private:
    std::unique_ptr<Buffer> m_buffer;

    std::vector<Buffer> m_slices;

    prev::util::CircularIndex<uint32_t> m_index{ 0 };
};
} // namespace prev::render::buffer

#endif // !__BUFFER_POOL_H__
