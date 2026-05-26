#include "Semaphore.h"

#include <stdexcept>

namespace prev::core::sync {
Semaphore::Semaphore(GfxDevice device, GfxSemaphoreType type, uint64_t initialValue, const char* label)
    : m_device{ device }
{
    GfxSemaphoreDescriptor desc{};
    desc.sType = GFX_STRUCTURE_TYPE_SEMAPHORE_DESCRIPTOR;
    desc.label = label;
    desc.type = type;
    desc.initialValue = initialValue;
    if (gfxDeviceCreateSemaphore(m_device, &desc, &m_semaphore) != GFX_RESULT_SUCCESS || !m_semaphore) {
        throw std::runtime_error("Failed to create semaphore");
    }
}

Semaphore::~Semaphore()
{
    if (m_semaphore) {
        gfxSemaphoreDestroy(m_semaphore);
    }
}

Semaphore::Semaphore(Semaphore&& other) noexcept
    : m_device{ other.m_device }
    , m_semaphore{ other.m_semaphore }
{
    other.m_semaphore = nullptr;
}

Semaphore& Semaphore::operator=(Semaphore&& other) noexcept
{
    if (this != &other) {
        if (m_semaphore) {
            gfxSemaphoreDestroy(m_semaphore);
        }
        m_device = other.m_device;
        m_semaphore = other.m_semaphore;
        other.m_semaphore = nullptr;
    }
    return *this;
}

Semaphore::operator GfxSemaphore() const
{
    return m_semaphore;
}
} // namespace prev::core::sync
