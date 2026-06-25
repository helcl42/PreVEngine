#include "DeferredResourceDestroyer.h"

#include "../common/Logger.h"

namespace prev::core {

DeferredResourceDestroyer::~DeferredResourceDestroyer()
{
    // Release all remaining resources on shutdown.
    m_destroyQueues.clear();
}

void DeferredResourceDestroyer::AdvanceFrame(uint32_t frameIndex)
{
    m_frameLoopActive = true;

    std::lock_guard<std::mutex> lock(m_destroyMutex);
    m_currentFrame = frameIndex;

    // Retire the slot we're about to reuse — its fence was waited, so the GPU is done. Keyed by slot
    // index directly (no modulus), so any index pattern the swapchain produces is correct.
    if (frameIndex >= m_destroyQueues.size()) {
        m_destroyQueues.resize(frameIndex + 1);
    }
    auto& retireQueue = m_destroyQueues[frameIndex];
    if (!retireQueue.empty()) {
        LOGI("DeferredResourceDestroyer::AdvanceFrame(%u) - retiring %zu resources", frameIndex, retireQueue.size());
    }
    retireQueue.clear(); // Destructors fire here via captured unique_ptrs.
}

bool DeferredResourceDestroyer::IsActive() const
{
    return m_frameLoopActive;
}

void DeferredResourceDestroyer::RetireAll()
{
    std::lock_guard<std::mutex> lock(m_destroyMutex);
    size_t total = 0;
    for (auto& queue : m_destroyQueues) {
        total += queue.size();
        queue.clear(); // Destructors fire here, releasing GPU resources.
    }
    if (total > 0) {
        LOGI("DeferredResourceDestroyer::RetireAll - released %zu deferred resources", total);
    }
}

void DeferredResourceDestroyer::Flush()
{
    m_frameLoopActive = false;
    RetireAll();
}

} // namespace prev::core
