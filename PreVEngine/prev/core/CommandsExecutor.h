#ifndef __COMMANDS_EXECUTOR_H__
#define __COMMANDS_EXECUTOR_H__

#include "device/Device.h"

#include "../util/VkUtils.h"

#include <functional>

namespace prev::core {
class CommandsExecutor final {
public:
    CommandsExecutor(device::Device& device, device::Queue& queue);

    ~CommandsExecutor();

public:
    void ExecuteImmediate(const std::function<void(VkCommandBuffer)>& func);

private:
    prev::core::device::Device& m_device;

    prev::core::device::Queue& m_queue;

    VkCommandPool m_immediateCommandPool{};

    VkCommandBuffer m_immediateCommandBuffer{};

    VkFence m_fence{};
};
} // namespace prev::core

#endif