#ifndef __OWNED_GFX_BUFFER_H__
#define __OWNED_GFX_BUFFER_H__

#include "../../core/Core.h" // GfxBuffer + gfxBufferDestroy

namespace prev::render::buffer {
// Move-only RAII owner of a raw GfxBuffer that destroys it when the owner dies. Handed to the
// deferred resource manager so a buffer (or a staging buffer) outlives any in-flight GPU work that
// still references it.
class OwnedGfxBuffer final {
public:
    explicit OwnedGfxBuffer(GfxBuffer buffer)
        : m_buffer{ buffer }
    {
    }

    OwnedGfxBuffer(OwnedGfxBuffer&& other) noexcept
        : m_buffer{ other.m_buffer }
    {
        other.m_buffer = nullptr;
    }

    OwnedGfxBuffer& operator=(OwnedGfxBuffer&&) = delete;
    OwnedGfxBuffer(const OwnedGfxBuffer&) = delete;
    OwnedGfxBuffer& operator=(const OwnedGfxBuffer&) = delete;

    ~OwnedGfxBuffer()
    {
        if (m_buffer) {
            gfxBufferDestroy(m_buffer);
        }
    }

private:
    GfxBuffer m_buffer{};
};
} // namespace prev::render::buffer

#endif // !__OWNED_GFX_BUFFER_H__
