#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include "../../core/device/Device.h"

namespace prev::render::framebuffer {
class FramebufferBuilder;

class Framebuffer final {
private:
    Framebuffer(GfxDevice device);

public:
    ~Framebuffer();

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer(Framebuffer&& other) noexcept;
    Framebuffer& operator=(Framebuffer&& other) noexcept;

public:
    operator GfxFramebuffer() const;

public:
    friend class FramebufferBuilder;

private:
    GfxDevice m_device{};
    GfxFramebuffer m_framebuffer{};
};
} // namespace prev::render::framebuffer

#endif
