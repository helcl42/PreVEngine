#include "XrTimeProvider.h"

#ifdef ENABLE_XR

namespace prev::xr {
XrTimeProvider::XrTimeProvider(const IXr& xr)
    : m_xr{ xr }
{
}

void XrTimeProvider::Update()
{
}

float XrTimeProvider::GetDelta() const
{
    return m_xr.GetCurrentDeltaTime();
}
} // namespace prev::xr

#endif // ENABLE_XR
