#include "XrFactory.h"

#ifdef ENABLE_XR

#include "open_xr/OpenXr.h"
#include "open_xr/OpenXrLoader.h"

namespace prev::xr {
std::unique_ptr<IXr> XrFactory::Create() const
{
    if (!open_xr::LoadOpenXr()) {
        throw std::runtime_error("Could not load OpenXR.");
    }
    return std::make_unique<open_xr::OpenXr>();
}
} // namespace prev::xr

#endif