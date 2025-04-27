#ifndef __OPEN_XR_CONTEXT_H__
#define __OPEN_XR_CONTEXT_H__

#ifdef ENABLE_XR

#include "OpenXrCommon.h"

namespace prev::xr::common {
    struct OpenXrContext {
        XrInstance instance{ XR_NULL_HANDLE };

        XrSystemId systemId{ XR_NULL_SYSTEM_ID };

        XrSession session{ XR_NULL_HANDLE };

        XrSpace localSpace{ XR_NULL_HANDLE };
    };
}

#endif

#endif //__OPEN_XR_CONTEXT_H__
