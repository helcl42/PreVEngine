#include "XrTimeProvider.h"

#ifdef ENABLE_XR

#include <utility>

namespace prev::xr {
XrTimeProvider::XrTimeProvider(const IXr& xr, std::unique_ptr<prev::time::ITimeProvider> windowTime)
    : m_xr{ xr }
    , m_windowTime{ std::move(windowTime) }
{
}

void XrTimeProvider::Update()
{
    m_windowTime->Update();
}

float XrTimeProvider::GetDelta() const
{
    return m_xr.IsSessionRunning() ? m_xr.GetCurrentDeltaTime() : m_windowTime->GetDelta();
}
} // namespace prev::xr

#endif // ENABLE_XR
