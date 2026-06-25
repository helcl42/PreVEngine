#include "DeferredResourceUploader.h"

#include "../common/Logger.h"

#include <memory>
#include <utility>

namespace prev::core {
namespace {
    // Move-only RAII owner of a staging GfxBuffer, destroyed when it dies. Handed to the destroyer so the
    // staging outlives the just-recorded copy. Kept core-local (operates on raw Gfx handles only) to avoid
    // a core -> render dependency on prev::render::buffer::OwnedGfxBuffer.
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
    // Anything never flushed (e.g. a resource built async right before shutdown) still owns a staging
    // buffer; the caller has waited the GPU idle by now, so free them directly.
    for (auto& entry : m_pending) {
        if (entry.staging) {
            gfxBufferDestroy(entry.staging);
        }
    }
    m_pending.clear();
}

void DeferredResourceUploader::Enqueue(std::function<void(GfxCommandEncoder)> record, std::shared_ptr<std::atomic<ResourceState>> state, GfxBuffer staging)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pending.push_back(Entry{ std::move(record), std::move(state), staging });
}

void DeferredResourceUploader::Flush(GfxCommandEncoder encoder)
{
    std::vector<Entry> pending;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        pending.swap(m_pending);
    }

    if (pending.empty()) {
        return;
    }

    LOGI("DeferredResourceUploader::Flush - recording %zu async uploads", pending.size());

    for (auto& entry : pending) {
        // Only record for resources still pending creation. A resource dropped before this flush is
        // marked Destroying by its destructor — skip its work entirely so we never record into / touch a
        // destroyed resource (the record closure may capture a now-dangling resource pointer).
        const ResourceState state{ entry.state ? entry.state->load() : ResourceState::Creating };
        if (state == ResourceState::Creating) {
            if (entry.record) {
                entry.record(encoder);
            }
            // Ready from this frame on: the copy is now recorded before any sampling in this command buffer.
            entry.state->store(ResourceState::Ready);
        }
        if (entry.staging) {
            // Outlive the just-recorded copy (or simply free it if cancelled): retired once this frame's
            // fence is waited.
            m_destroyer.Destroy(std::make_unique<StagingBuffer>(entry.staging));
        }
    }
}

} // namespace prev::core
