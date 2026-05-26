#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include "../device/Device.h"

namespace prev::core::sync {
class Semaphore final {
public:
    Semaphore(GfxDevice device, GfxSemaphoreType type = GFX_SEMAPHORE_TYPE_BINARY, uint64_t initialValue = 0, const char* label = nullptr);

    ~Semaphore();

    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    Semaphore(Semaphore&& other) noexcept;
    Semaphore& operator=(Semaphore&& other) noexcept;

public:
    operator GfxSemaphore() const;

private:
    GfxDevice m_device{};
    GfxSemaphore m_semaphore{};
};
} // namespace prev::core::sync

#endif
