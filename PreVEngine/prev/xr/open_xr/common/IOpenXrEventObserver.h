#ifndef __IOPEN_XR_EVENT_OBSERVER_H__
#define __IOPEN_XR_EVENT_OBSERVER_H__

#ifdef ENABLE_XR

#include "OpenXrCommon.h"

namespace prev::xr::open_xr::common {
class IOpenXrEventObserver {
public:
    virtual void OnEvent(const XrEventDataBuffer& evt) = 0;

public:
    virtual ~IOpenXrEventObserver() = default;
};
} // namespace prev::xr::open_xr::common

#endif

#endif //__IOPEN_XR_EVENT_OBSERVER_H__
