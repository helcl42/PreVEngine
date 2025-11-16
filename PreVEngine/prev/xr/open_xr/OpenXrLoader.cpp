#include "OpenXrLoader.h"

#ifdef ENABLE_XR

#include "common/OpenXrCommon.h"
#include "util/OpenXrUtils.h"

#include "../../common/Logger.h"

#ifdef TARGET_PLATFORM_ANDROID
#include <android_native.h>
#endif

#include <stdexcept>

namespace prev::xr::open_xr {
OpenXrLoader::OpenXrLoader()
{
#ifdef TARGET_PLATFORM_ANDROID
    // https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#XR_KHR_loader_init
    // Load xrInitializeLoaderKHR() function pointer. On Android, the loader must be initialized with variables from android_app *.
    // Without this, there's is no loader and thus our function calls to OpenXR would fail.
    PFN_xrInitializeLoaderKHR xrInitializeLoaderKHR{};
    XrResult res = xrGetInstanceProcAddr(XR_NULL_HANDLE, "xrInitializeLoaderKHR", (PFN_xrVoidFunction*)&xrInitializeLoaderKHR);
    if (!XR_SUCCEEDED(res)) {
        throw std::runtime_error("Failed to get InstanceProcAddr for xrInitializeLoaderKHR.");
    }
    if (!xrInitializeLoaderKHR) {
        throw std::runtime_error("Failed to get xrInitializeLoaderKHR.");
    }

    // Fill out an XrLoaderInitInfoAndroidKHR structure and initialize the loader for Android.
    XrLoaderInitInfoAndroidKHR loaderInitializeInfoAndroid{ util::CreateStruct<XrLoaderInitInfoAndroidKHR>(XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR) };
    loaderInitializeInfoAndroid.applicationVM = android_native_get_app_instance()->activity->vm;
    loaderInitializeInfoAndroid.applicationContext = android_native_get_app_instance()->activity->clazz;
    res = xrInitializeLoaderKHR((XrLoaderInitInfoBaseHeaderKHR*)&loaderInitializeInfoAndroid);
    if (!XR_SUCCEEDED(res)) {
        throw std::runtime_error("Failed to initialize Loader for Android.");
    }
#endif
}

bool OpenXrLoader::IsLoaded() const
{
    return true;
}
} // namespace prev::xr::open_xr

#endif