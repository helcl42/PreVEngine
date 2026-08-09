#include "DeferredResourceUploader.h"

#include "OwnedGfxHandle.h"

#include "../common/Logger.h"

#include <iterator>
#include <memory>
#include <utility>

namespace prev::core {
namespace {
    // Max bytes of upload work recorded into one frame's command buffer.
    constexpr uint64_t MaxUploadBytesPerFlush{ 8ull * 1024 * 1024 };

    // Max bytes of queued-but-not-yet-flushed staging memory. Each async upload holds a host-visible copy
    // of its data until flushed. Beyond this, builders fall back to synchronous uploads, which free their
    // staging immediately.
    constexpr uint64_t MaxOutstandingUploadBytes{ 64ull * 1024 * 1024 };
} // namespace

DeferredResourceUploader::DeferredResourceUploader(DeferredResourceDestroyer& destroyer)
    : m_destroyer{ destroyer }
{
}

DeferredResourceUploader::~DeferredResourceUploader()
{
    // Never-flushed entries still own a staging buffer; the GPU is idle by shutdown, so free directly.
    for (auto& entry : m_pending) {
        if (entry.staging.buffer) {
            gfxBufferDestroy(entry.staging.buffer);
        }
    }
    m_pending.clear();
}

void DeferredResourceUploader::Enqueue(std::function<void(GfxCommandEncoder)> record, std::shared_ptr<std::atomic<ResourceState>> state, StagingData staging)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_outstandingBytes.fetch_add(staging.bytes, std::memory_order_relaxed);
    m_pending.push_back(Entry{ std::move(record), std::move(state), std::move(staging) });
}

bool DeferredResourceUploader::HasPending() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return !m_pending.empty();
}

bool DeferredResourceUploader::CanQueue(uint64_t bytes) const
{
    return m_outstandingBytes.load(std::memory_order_relaxed) + bytes <= MaxOutstandingUploadBytes;
}

void DeferredResourceUploader::Flush(GfxCommandEncoder encoder)
{
    std::vector<Entry> batch;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        // Byte-budgeted FIFO batch of the ready entries (at least one); unprepared staging stays queued.
        std::vector<Entry> keep;
        uint64_t budget{ 0 };
        for (auto& entry : m_pending) {
            const bool wantMore{ batch.empty() || budget + entry.staging.bytes <= MaxUploadBytesPerFlush };
            if (!wantMore || (entry.staging.prepare && !entry.staging.prepare())) {
                keep.push_back(std::move(entry));
                continue;
            }
            budget += entry.staging.bytes;
            batch.push_back(std::move(entry));
        }
        m_pending = std::move(keep);
    }

    if (batch.empty()) {
        return;
    }

    LOGI("DeferredResourceUploader::Flush - recording %zu async uploads", batch.size());

    for (auto& entry : batch) {
        m_outstandingBytes.fetch_sub(entry.staging.bytes, std::memory_order_relaxed); // staging is about to be freed
        // CAS Creating -> Ready: skips a resource dropped before flush (destructor set Destroying) and
        // never clobbers that Destroying. Records are handle-only, so a concurrent drop is not a UAF.
        ResourceState expected{ ResourceState::Creating };
        const bool claimed{ entry.state && entry.state->compare_exchange_strong(expected, ResourceState::Ready) };
        if (claimed && entry.record) {
            entry.record(encoder);
        }
        if (entry.staging.buffer) {
            // Defer-destroy so the staging outlives the just-recorded copy (or just free it if cancelled).
            m_destroyer.Destroy(std::make_unique<OwnedGfxBuffer>(entry.staging.buffer));
        }
    }
}

} // namespace prev::core
