#ifndef __OPENXR_LOADER_H__
#define __OPENXR_LOADER_H__

#ifdef ENABLE_OPENXR

namespace prev::xr::open_xr {
// Per run, not per process: Android may start a new activity in the same process, and the loader must not
// keep the destroyed one's context.
class OpenXrLoader final {
public:
    OpenXrLoader();

    ~OpenXrLoader() = default;
};
} // namespace prev::xr::open_xr

#endif

#endif