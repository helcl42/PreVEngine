#ifndef __COMMANDS_EXECUTOR_H__
#define __COMMANDS_EXECUTOR_H__

#include "DeferredResourceDestroyer.h"
#include "device/Device.h"

#include "../core/Core.h"

#include <functional>

namespace prev::core {
class CommandsExecutor final {
public:
    CommandsExecutor(const device::Device& device, const device::Queue& queue);

    ~CommandsExecutor() = default;

public:
    void ExecuteImmediate(const std::function<void(GfxCommandEncoder)>& func);

    void ExecuteDeferred(DeferredResourceDestroyer& destroyer, const std::function<void(GfxCommandEncoder)>& func);

private:
    GfxCommandEncoder RecordCommands(const std::function<void(GfxCommandEncoder)>& func) const;

    const prev::core::device::Device& m_device;

    const prev::core::device::Queue& m_queue;
};
} // namespace prev::core

#endif
