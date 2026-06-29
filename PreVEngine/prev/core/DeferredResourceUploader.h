#ifndef __DEFERRED_RESOURCE_UPLOADER_H__
#define __DEFERRED_RESOURCE_UPLOADER_H__

#include "Core.h"
#include "DeferredResourceDestroyer.h"
#include "ResourceState.h"

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace prev::core {

// Queues async resource uploads (from BuildAsync) and records a byte-budgeted batch into the frame's
// command encoder once per frame, before rendering. On flush each resource flips Creating -> Ready and its
// staging buffer is defer-destroyed. Recording before the render passes keeps the uploads stall-free (the
// resource is valid the same frame) without a separate submission.
class DeferredResourceUploader final {
public:
    explicit DeferredResourceUploader(DeferredResourceDestroyer& destroyer);

    ~DeferredResourceUploader();

    DeferredResourceUploader(const DeferredResourceUploader&) = delete;
    DeferredResourceUploader& operator=(const DeferredResourceUploader&) = delete;

public:
    // Queues upload work, the resource's shared lifecycle state, its staging buffer (may be null), and the
    // upload size in bytes (used to budget per-frame work). Thread-safe; replayed at the next Flush.
    void Enqueue(std::function<void(GfxCommandEncoder)> record, std::shared_ptr<std::atomic<ResourceState>> state, GfxBuffer staging, uint64_t bytes);

    // Records a byte-budgeted batch of still-Creating uploads into `encoder` (flipping each Ready), leaving
    // the rest queued for later frames. Called at frame start, before rendering and outside a render pass.
    void Flush(GfxCommandEncoder encoder);

    // True if queuing an upload of `bytes` would keep the outstanding (queued-but-not-yet-flushed) staging
    // memory within budget; builders fall back to a synchronous upload when this returns false.
    bool CanQueue(uint64_t bytes) const;

private:
    struct Entry {
        std::function<void(GfxCommandEncoder)> record;
        std::shared_ptr<std::atomic<ResourceState>> state;
        GfxBuffer staging{};
        uint64_t bytes{};
    };

    DeferredResourceDestroyer& m_destroyer;

    std::vector<Entry> m_pending;
    std::mutex m_mutex;
    std::atomic<uint64_t> m_outstandingBytes{ 0 };
};

} // namespace prev::core

#endif // !__DEFERRED_RESOURCE_UPLOADER_H__
