#include "XrFactory.h"

#ifdef ENABLE_XR

#include "open_xr/OpenXr.h"

namespace prev::xr {
std::unique_ptr<IXr> XrFactory::Create() const
{
    return std::make_unique<open_xr::OpenXr>();
}
} // namespace prev::xr

#endif