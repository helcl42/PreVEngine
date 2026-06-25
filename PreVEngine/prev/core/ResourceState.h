#ifndef __RESOURCE_STATE_H__
#define __RESOURCE_STATE_H__

namespace prev::core {

// Lifecycle state of a GPU resource (Buffer, ImageBuffer). Lives in core so the DeferredResourceUploader
// (core) and the resource types / IResource (render) can both refer to it without a core -> render
// dependency.
enum class ResourceState {
    // Allocated, but no content has been uploaded and no upload is scheduled (e.g. a render target or a
    // compute output, whose content is produced by a pass rather than an upload). Not "ready" in the
    // populated sense; its validity is the producing pass's responsibility, not the readiness gate's.
    None,

    // An asynchronous upload is in flight (queued with the DeferredResourceUploader, not yet flushed).
    // The resource is allocated but its contents are undefined — consumers must skip it.
    Creating,

    // Content has been uploaded (or the resource was built synchronously / into a caller's encoder).
    // Safe to use.
    Ready,

    // The resource was dropped before its pending async upload was flushed. The uploader uses this to
    // cancel the upload (it must not record work for, or otherwise touch, a destroyed resource).
    Destroying,
};

} // namespace prev::core

#endif // !__RESOURCE_STATE_H__
