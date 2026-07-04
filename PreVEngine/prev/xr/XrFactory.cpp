#include "XrFactory.h"

#ifdef ENABLE_XR

#if defined(ENABLE_WEBXR)
#include "web_xr/WebXr.h"
#elif defined(ENABLE_OPENXR)
#include "open_xr/OpenXr.h"
#endif

namespace prev::xr {
std::unique_ptr<IXr> XrFactory::Create(prev::core::engine::XrMode xrMode) const
{
#if defined(ENABLE_WEBXR)
    return std::make_unique<web_xr::WebXr>(xrMode);
#elif defined(ENABLE_OPENXR)
    return std::make_unique<open_xr::OpenXr>(xrMode);
#else
#error "Not implemented XR backend!"
#endif
}
} // namespace prev::xr

#endif
