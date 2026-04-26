#include "Instance.h"

#include "../../common/Logger.h"

namespace prev::core::instance {
Instance::Instance(GfxInstance instance)
    : m_instance{ instance }
{
}

Instance::~Instance()
{
    gfxInstanceDestroy(m_instance);
    m_instance = nullptr;
    LOGI("GFX Instance destroyed");
}

GfxInstance Instance::GetHandle() const
{
    return m_instance;
}

Instance::operator GfxInstance() const
{
    return m_instance;
}
} // namespace prev::core::instance
