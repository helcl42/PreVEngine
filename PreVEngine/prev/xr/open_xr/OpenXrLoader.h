#ifndef __OPENXR_LOADER_H__
#define __OPENXR_LOADER_H__

#ifdef ENABLE_XR

#include "../../common/pattern/Singleton.h"

namespace prev::xr::open_xr {
class OpenXrLoader final : public prev::common::pattern::Singleton<OpenXrLoader> {
public:
    OpenXrLoader();

    ~OpenXrLoader() = default;

public:
    bool IsLoaded() const;

private:
    friend class prev::common::pattern::Singleton<OpenXrLoader>;
};
} // namespace prev::xr::open_xr

#endif

#endif