#ifndef __OWNED_GFX_HANDLE_H__
#define __OWNED_GFX_HANDLE_H__

#include "Core.h"

namespace prev::core {
// Move-only RAII owner of a raw Gfx handle; destroys it when the owner dies.
template <typename THandle, auto DestroyFn>
class OwnedGfxHandle final {
public:
    OwnedGfxHandle() noexcept = default;

    explicit OwnedGfxHandle(THandle handle)
        : m_handle{ handle }
    {
    }

    OwnedGfxHandle(OwnedGfxHandle&& other) noexcept
        : m_handle{ other.m_handle }
    {
        other.m_handle = nullptr;
    }

    OwnedGfxHandle& operator=(OwnedGfxHandle&& other) noexcept
    {
        if (this != &other) {
            if (m_handle) {
                DestroyFn(m_handle);
            }
            m_handle = other.m_handle;
            other.m_handle = nullptr;
        }
        return *this;
    }

    OwnedGfxHandle(const OwnedGfxHandle&) = delete;
    OwnedGfxHandle& operator=(const OwnedGfxHandle&) = delete;

    ~OwnedGfxHandle()
    {
        if (m_handle) {
            DestroyFn(m_handle);
        }
    }

    operator THandle() const { return m_handle; }

private:
    THandle m_handle{};
};

using OwnedGfxBuffer = OwnedGfxHandle<GfxBuffer, gfxBufferDestroy>;
using OwnedGfxCommandEncoder = OwnedGfxHandle<GfxCommandEncoder, gfxCommandEncoderDestroy>;
using OwnedGfxTexture = OwnedGfxHandle<GfxTexture, gfxTextureDestroy>;
using OwnedGfxTextureView = OwnedGfxHandle<GfxTextureView, gfxTextureViewDestroy>;
} // namespace prev::core

#endif // !__OWNED_GFX_HANDLE_H__
