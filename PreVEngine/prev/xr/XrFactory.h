#ifndef __XR_FACTORY_H__
#define __XR_FACTORY_H__

#ifdef ENABLE_XR

#include "IXr.h"

#include <memory>

namespace prev::xr {
class XrFactory final {
public:
    std::unique_ptr<IXr> Create() const;
};
} // namespace prev::xr

#endif

#endif