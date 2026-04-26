#ifndef __COMMANDS_EXECUTOR_H__
#define __COMMANDS_EXECUTOR_H__

#include "device/Device.h"

#include "../core/Core.h"

#include <functional>

namespace prev::core {
class CommandsExecutor final {
public:
    CommandsExecutor(const device::Device& device, const device::Queue& queue);

    ~CommandsExecutor();

public:
    void ExecuteImmediate(const std::function<void(GfxCommandEncoder)>& func);

private:
    const prev::core::device::Device& m_device;

    const prev::core::device::Queue& m_queue;

    GfxCommandEncoder m_commandEncoder{};

    GfxFence m_fence{};
};
} // namespace prev::core

#endif
