#ifndef __OPEN_XR_COMMON_H__
#define __OPEN_XR_COMMON_H__

#ifdef ENABLE_XR

#include "../../common/Common.h"
#include "../../common/Logger.h"
#include "../../core/Core.h"

#if defined(TARGET_PLATFORM_ANDROID)
#define XR_USE_PLATFORM_ANDROID
#elif defined(TARGET_PLATFORM_WINDOWS)
#define XR_USE_PLATFORM_WIN32
#elif defined(TARGET_PLATFORM_LINUX)
#define XR_USE_PLATFORM_XCB
#else
#error Unsupported XR platform.
#endif

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

#endif

#endif //__OPEN_XR_COMMON_H__
