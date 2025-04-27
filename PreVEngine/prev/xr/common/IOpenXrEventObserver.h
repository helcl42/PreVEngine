#ifndef __IOPEN_XR_EVENT_OBSERVER_H__
#define __IOPEN_XR_EVENT_OBSERVER_H__

#ifdef ENABLE_XR

#include "OpenXrCommon.h"

namespace prev::xr::common {
class IOpenXrEventObserver {
public:
    virtual void OnOpenXrEvent(const XrEventDataBuffer& evt) = 0;

public:
    virtual ~IOpenXrEventObserver() = default;
};
}

#endif

#endif //__IOPEN_XR_EVENT_OBSERVER_H__
