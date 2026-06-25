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

// Collects asynchronous resource uploads and records them into a frame's command encoder at a single
// safe point each frame (before any rendering). A resource built via BuildAsync is allocated immediately
// but left unpopulated and flagged not-ready; its upload work is queued here. On Flush the work is
// recorded into the frame encoder, the resource is flagged ready (its copy now precedes any sampling in
// the same command buffer), and the staging buffer is handed to the destroyer to outlive that copy.
class DeferredResourceUploader final {
public:
    explicit DeferredResourceUploader(DeferredResourceDestroyer& destroyer);

    ~DeferredResourceUploader();

    DeferredResourceUploader(const DeferredResourceUploader&) = delete;
    DeferredResourceUploader& operator=(const DeferredResourceUploader&) = delete;

public:
    // Queues creation GPU work, the resource's shared lifecycle state, and the staging buffer that backs
    // it. Thread-safe: called from whatever thread builds the resource. The work is replayed at the next
    // Flush. `staging` may be null (e.g. a layout-only transition with no data to copy).
    void Enqueue(std::function<void(GfxCommandEncoder)> record, std::shared_ptr<std::atomic<ResourceState>> state, GfxBuffer staging);

    // For each queued upload: if the resource is still Creating, record its work into `encoder` and flag
    // it Ready; if it was dropped meanwhile (Destroying) skip the work entirely (never touch a destroyed
    // resource). Either way the staging buffer is defer-destroyed. Called by the engine at frame start,
    // before rendering and outside a render pass (copies/barriers are illegal inside one). Drains the queue.
    void Flush(GfxCommandEncoder encoder);

private:
    struct Entry {
        std::function<void(GfxCommandEncoder)> record;
        std::shared_ptr<std::atomic<ResourceState>> state;
        GfxBuffer staging{};
    };

    DeferredResourceDestroyer& m_destroyer;

    std::vector<Entry> m_pending;
    std::mutex m_mutex;
};

} // namespace prev::core

#endif // !__DEFERRED_RESOURCE_UPLOADER_H__
