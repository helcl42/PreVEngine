#ifndef __OPENXR_LOADER_H__
#define __OPENXR_LOADER_H__

#ifdef ENABLE_XR

namespace prev::open_xr::xr {
#ifdef TARGET_PLATFORM_ANDROID
bool LoadOpenXr(void* vm, void* clazz);
#endif
} // namespace prev::open_xr::xr

#endif

#endif