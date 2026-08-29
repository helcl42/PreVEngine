#ifndef __XR_TIME_PROVIDER_H__
#define __XR_TIME_PROVIDER_H__

#ifdef ENABLE_XR

#include "IXr.h"

#include "../time/ITimeProvider.h"

namespace prev::xr {
class XrTimeProvider final : public prev::time::ITimeProvider {
public:
    explicit XrTimeProvider(const IXr& xr);

public:
    void Update() override;

    float GetDelta() const override;

private:
    const IXr& m_xr;
};
} // namespace prev::xr

#endif // ENABLE_XR

#endif // !__XR_TIME_PROVIDER_H__
