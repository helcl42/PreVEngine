#ifndef __DEFERRED_RESOURCE_DESTROYER_H__
#define __DEFERRED_RESOURCE_DESTROYER_H__

#include "Core.h"

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace prev::core {

class DeferredResourceDestroyer final {
public:
    DeferredResourceDestroyer() = default;

    ~DeferredResourceDestroyer();

    DeferredResourceDestroyer(const DeferredResourceDestroyer&) = delete;
    DeferredResourceDestroyer& operator=(const DeferredResourceDestroyer&) = delete;

public:
    // -----------------------------------------------------------------------
    // Deferred Destroy
    // -----------------------------------------------------------------------
    // Takes ownership of a GPU resource and destroys it when the current frame slot is next reused — the
    // GPU is done with it by then. Thread-safe.
    template <typename T>
    void Destroy(std::unique_ptr<T> resource)
    {
        if (!resource) {
            return;
        }

        // Type-erase ownership: wrap in shared_ptr so the lambda is copyable
        // (std::function requires copy-constructibility).
        // The shared_ptr destructor will destroy the resource when the last copy is gone.
        auto shared = std::shared_ptr<T>(std::move(resource));
        auto destroyer = [shared]() mutable {
            shared.reset();
        };

        std::lock_guard<std::mutex> lock(m_destroyMutex);
        if (m_currentFrame >= m_destroyQueues.size()) {
            m_destroyQueues.resize(m_currentFrame + 1);
        }
        m_destroyQueues[m_currentFrame].emplace_back(std::move(destroyer));
    }

    // -----------------------------------------------------------------------
    // Frame Lifecycle (called by Engine)
    // -----------------------------------------------------------------------

    // Retires resources deferred during this slot's previous use. The engine has waited the slot's fence,
    // so the GPU is done. Self-sizes to the slot indices it's given — no frames-in-flight count needed.
    void AdvanceFrame(uint32_t frameIndex);

    // Returns true once the frame loop is running (after the first AdvanceFrame call).
    bool IsActive() const;

    // Frees every deferred-destroy resource immediately, regardless of slot. Caller MUST ensure the GPU
    // is idle. Used on swapchain recreation so resources keyed to slots the new swapchain may never
    // revisit aren't stranded until shutdown.
    void RetireAll();

    // Flushes all pending deferred-destroy resources immediately and marks the loop inactive.
    // Call during shutdown AFTER the GPU is idle and BEFORE the device is destroyed.
    void Flush();

private:
    uint32_t m_currentFrame{};

    // Read from arbitrary threads (buffer destructors / builders via IsActive), written by the render
    // thread — atomic so the concurrent access is well-defined.
    std::atomic<bool> m_frameLoopActive{ false };

    // Destroy queues indexed by frame slot (grown on demand); retired when AdvanceFrame revisits the slot.
    std::vector<std::vector<std::function<void()>>> m_destroyQueues;
    std::mutex m_destroyMutex;
};

} // namespace prev::core

#endif // !__DEFERRED_RESOURCE_DESTROYER_H__
