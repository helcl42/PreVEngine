#ifndef __XR_TIME_PROVIDER_H__
#define __XR_TIME_PROVIDER_H__

#ifdef ENABLE_XR

#include "IXr.h"

#include "../time/ITimeProvider.h"

#include <memory>

namespace prev::xr {
// The session's frame timing while it runs, the window's clock otherwise.
class XrTimeProvider final : public prev::time::ITimeProvider {
public:
    XrTimeProvider(const IXr& xr, std::unique_ptr<prev::time::ITimeProvider> windowTime);

public:
    void Update() override;

    float GetDelta() const override;

private:
    const IXr& m_xr;

    std::unique_ptr<prev::time::ITimeProvider> m_windowTime;
};
} // namespace prev::xr

#endif // ENABLE_XR

#endif // !__XR_TIME_PROVIDER_H__
