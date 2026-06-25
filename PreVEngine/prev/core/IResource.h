#ifndef __I_RESOURCE_H__
#define __I_RESOURCE_H__

#include "ResourceState.h"

namespace prev::core {

// Common interface for any resource whose population may be deferred. A resource built synchronously is
// Ready immediately; one built asynchronously is Creating until its upload is flushed. Consumers must skip
// a resource until it reports ready (its contents are otherwise undefined). Generic (not render-specific):
// it merely exposes a ResourceState lifecycle; GPU buffers/textures are its current implementers.
class IResource {
public:
    virtual ~IResource() = default;

    virtual ResourceState GetState() const = 0;

    bool IsReady() const { return GetState() == ResourceState::Ready; }
};

} // namespace prev::core

#endif // !__I_RESOURCE_H__
