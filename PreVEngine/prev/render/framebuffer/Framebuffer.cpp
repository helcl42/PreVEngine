#include "Framebuffer.h"

namespace prev::render::framebuffer {
Framebuffer::Framebuffer(GfxDevice device)
    : m_device{ device }
{
}

Framebuffer::~Framebuffer()
{
    if (m_framebuffer) {
        gfxFramebufferDestroy(m_framebuffer);
    }
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept
    : m_device{ other.m_device }
    , m_framebuffer{ other.m_framebuffer }
{
    other.m_framebuffer = nullptr;
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
{
    if (this != &other) {
        if (m_framebuffer) {
            gfxFramebufferDestroy(m_framebuffer);
        }
        m_device = other.m_device;
        m_framebuffer = other.m_framebuffer;
        other.m_framebuffer = nullptr;
    }
    return *this;
}

Framebuffer::operator GfxFramebuffer() const
{
    return m_framebuffer;
}
} // namespace prev::render::framebuffer
