#ifndef __OPEN_XR_COMMON_H__
#define __OPEN_XR_COMMON_H__

#include "../core/Core.h"
#include "../common/Logger.h"

// TODO - switch platforms here
#define XR_USE_PLATFORM_ANDROID

#define XR_USE_GRAPHICS_API_VULKAN
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

#define OPENXR_CHECK(x, y)                            \
    {                                                 \
        XrResult result = (x);                        \
        if (!XR_SUCCEEDED(result)) {                  \
            LOGE("OpenXR: %d - %s.", int(result), y); \
        }                                             \
    }

#endif //__OPEN_XR_COMMON_H__
