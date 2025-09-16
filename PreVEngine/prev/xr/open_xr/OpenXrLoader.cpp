#include "OpenXrLoader.h"

#ifdef ENABLE_XR

#include "common/OpenXrCommon.h"

#include "../../common/Logger.h"

namespace prev::open_xr::xr {
#ifdef TARGET_PLATFORM_ANDROID
bool LoadOpenXr(void* vm, void* clazz)
{
    // https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#XR_KHR_loader_init
    // Load xrInitializeLoaderKHR() function pointer. On Android, the loader must be initialized with variables from android_app *.
    // Without this, there's is no loader and thus our function calls to OpenXR would fail.
    PFN_xrInitializeLoaderKHR xrInitializeLoaderKHR{};
    XrResult res = xrGetInstanceProcAddr(XR_NULL_HANDLE, "xrInitializeLoaderKHR", (PFN_xrVoidFunction*)&xrInitializeLoaderKHR);
    if (!XR_SUCCEEDED(res)) {
        LOGW("Failed to get InstanceProcAddr for xrInitializeLoaderKHR.");
        return false;
    }
    if (!xrInitializeLoaderKHR) {
        LOGW("Failed to get xrInitializeLoaderKHR.");
        return false;
    }

    // Fill out an XrLoaderInitInfoAndroidKHR structure and initialize the loader for Android.
    XrLoaderInitInfoAndroidKHR loaderInitializeInfoAndroid{ XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR };
    loaderInitializeInfoAndroid.applicationVM = vm;
    loaderInitializeInfoAndroid.applicationContext = clazz;
    res = xrInitializeLoaderKHR((XrLoaderInitInfoBaseHeaderKHR*)&loaderInitializeInfoAndroid);
    if (!XR_SUCCEEDED(res)) {
        LOGW("Failed to initialize Loader for Android.");
        return false;
    }
    return true;
}
#endif
} // namespace prev::open_xr::xr

#endif