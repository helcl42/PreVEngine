#include "XrFactory.h"

#ifdef ENABLE_XR

#include "open_xr/OpenXr.h"
#include "open_xr/OpenXrLoader.h"

namespace prev::xr {
std::unique_ptr<IXr> XrFactory::Create() const
{
#ifdef TARGET_PLATFORM_ANDROID
    if (!prev::open_xr::xr::LoadOpenXr(g_AndroidApp->activity->vm, g_AndroidApp->activity->clazz)) {
        throw std::runtime_error("Could not load OpenXR.");
    }
#endif
    return std::make_unique<open_xr::OpenXr>();
}
} // namespace prev::xr

#endif