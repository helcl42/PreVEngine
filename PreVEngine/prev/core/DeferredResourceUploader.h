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
    // The staging side of an upload: what the uploader destroys, budgets, and (optionally) readies.
    struct StagingData {
        GfxBuffer buffer{}; // may be null; owned by the uploader once enqueued
        uint64_t bytes{}; // what the flush budget counts
        std::function<bool()> prepare{}; // empty = filled; else call to retry (false = map not landed yet)
    };

    // Queues upload work and the resource's shared lifecycle state. Thread-safe; replayed at the next Flush.
    void Enqueue(std::function<void(GfxCommandEncoder)> record, std::shared_ptr<std::atomic<ResourceState>> state, StagingData staging);

    // Records a byte-budgeted batch of still-Creating uploads into `encoder` (flipping each Ready), leaving
    // the rest queued for later frames. Called at frame start, before rendering and outside a render pass.
    void Flush(GfxCommandEncoder encoder);

    bool HasPending() const;

    // True if queuing an upload of `bytes` would keep the outstanding (queued-but-not-yet-flushed) staging
    // memory within budget; builders fall back to a synchronous upload when this returns false.
    bool CanQueue(uint64_t bytes) const;

private:
    struct Entry {
        std::function<void(GfxCommandEncoder)> record;
        std::shared_ptr<std::atomic<ResourceState>> state;
        StagingData staging;
    };

    DeferredResourceDestroyer& m_destroyer;

    std::vector<Entry> m_pending;
    mutable std::mutex m_mutex;
    std::atomic<uint64_t> m_outstandingBytes{ 0 };
};

} // namespace prev::core

#endif // !__DEFERRED_RESOURCE_UPLOADER_H__
