#include "DeferredResourceUploader.h"

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

    // RAII owner of a staging GfxBuffer. Core-local (raw handles only) to avoid a core -> render dependency.
    class StagingBuffer final {
    public:
        explicit StagingBuffer(GfxBuffer buffer)
            : m_buffer{ buffer }
        {
        }
        StagingBuffer(const StagingBuffer&) = delete;
        StagingBuffer& operator=(const StagingBuffer&) = delete;
        ~StagingBuffer()
        {
            if (m_buffer) {
                gfxBufferDestroy(m_buffer);
            }
        }

    private:
        GfxBuffer m_buffer{};
    };
} // namespace

DeferredResourceUploader::DeferredResourceUploader(DeferredResourceDestroyer& destroyer)
    : m_destroyer{ destroyer }
{
}

DeferredResourceUploader::~DeferredResourceUploader()
{
    // Never-flushed entries still own a staging buffer; the GPU is idle by shutdown, so free directly.
    for (auto& entry : m_pending) {
        if (entry.staging) {
            gfxBufferDestroy(entry.staging);
        }
    }
    m_pending.clear();
}

void DeferredResourceUploader::Enqueue(std::function<void(GfxCommandEncoder)> record, std::shared_ptr<std::atomic<ResourceState>> state, GfxBuffer staging, uint64_t bytes)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pending.push_back(Entry{ std::move(record), std::move(state), staging, bytes });
    m_outstandingBytes.fetch_add(bytes, std::memory_order_relaxed);
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
        // Take a byte-budgeted prefix (FIFO), always at least one, leaving the rest for later frames.
        uint64_t budget{ 0 };
        size_t count{ 0 };
        while (count < m_pending.size() && (count == 0 || budget + m_pending[count].bytes <= MaxUploadBytesPerFlush)) {
            budget += m_pending[count].bytes;
            ++count;
        }
        batch.insert(batch.end(), std::make_move_iterator(m_pending.begin()), std::make_move_iterator(m_pending.begin() + count));
        m_pending.erase(m_pending.begin(), m_pending.begin() + count);
    }

    if (batch.empty()) {
        return;
    }

    LOGI("DeferredResourceUploader::Flush - recording %zu async uploads", batch.size());

    for (auto& entry : batch) {
        m_outstandingBytes.fetch_sub(entry.bytes, std::memory_order_relaxed); // staging is about to be freed
        // CAS Creating -> Ready: skips a resource dropped before flush (destructor set Destroying) and
        // never clobbers that Destroying. Records are handle-only, so a concurrent drop is not a UAF.
        ResourceState expected{ ResourceState::Creating };
        const bool claimed{ entry.state && entry.state->compare_exchange_strong(expected, ResourceState::Ready) };
        if (claimed && entry.record) {
            entry.record(encoder);
        }
        if (entry.staging) {
            // Defer-destroy so the staging outlives the just-recorded copy (or just free it if cancelled).
            m_destroyer.Destroy(std::make_unique<StagingBuffer>(entry.staging));
        }
    }
}

} // namespace prev::core
