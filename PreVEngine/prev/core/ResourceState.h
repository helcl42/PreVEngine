#ifndef __RESOURCE_STATE_H__
#define __RESOURCE_STATE_H__

namespace prev::core {

// Lifecycle state of a GPU resource. In core so both the uploader (core) and the resources (render) can
// use it without a core -> render dependency.
enum class ResourceState {
    None,       // allocated, no upload scheduled (e.g. render target / compute output) — not gate-ready
    Creating,   // async upload queued, not yet flushed — contents undefined, consumers must skip
    Ready,      // uploaded (or built synchronously) — safe to use
    Destroying, // dropped before its pending upload flushed — uploader cancels it
};

} // namespace prev::core

#endif // !__RESOURCE_STATE_H__
