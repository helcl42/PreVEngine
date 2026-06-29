#ifndef __FRAME_SUBMIT_SYNC_H__
#define __FRAME_SUBMIT_SYNC_H__

#include "../core/Core.h"

#include <vector>

namespace prev::render {

struct FrameSubmitSync {
    struct Wait {
        GfxSemaphore semaphore{};
        GfxPipelineStageFlags stage{};
        uint64_t value{}; // For timeline semaphores, the value to wait for. Ignored for binary semaphores.
    };

    struct Signal {
        GfxSemaphore semaphore{};
        uint64_t value{}; // For timeline semaphores, the value to signal to. Ignored for binary semaphores.
    };

    std::vector<Wait> waits;

    std::vector<Signal> signals;

    void AddWait(GfxSemaphore semaphore, GfxPipelineStageFlags stage, uint64_t value = 0)
    {
        waits.push_back({ semaphore, stage, value });
    }

    void AddSignal(GfxSemaphore semaphore, uint64_t value = 0)
    {
        signals.push_back({ semaphore, value });
    }
};

} // namespace prev::render

#endif // !__FRAME_SUBMIT_SYNC_H__
