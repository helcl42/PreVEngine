#ifndef __I_RESOURCE_H__
#define __I_RESOURCE_H__

#include "ResourceState.h"

namespace prev::core {

// Interface for any resource with a deferred-population lifecycle (ResourceState). Consumers must skip it
// until IsReady(). Generic, not render-specific; GPU buffers/textures are its current implementers.
class IResource {
public:
    virtual ~IResource() = default;

    virtual ResourceState GetState() const = 0;

    bool IsReady() const { return GetState() == ResourceState::Ready; }
};

} // namespace prev::core

#endif // !__I_RESOURCE_H__
