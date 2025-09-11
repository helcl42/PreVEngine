#include "CommandsExecutor.h"

#include "../util/VkUtils.h"

namespace prev::core {
CommandsExecutor::CommandsExecutor(const device::Device& device, const device::Queue& queue)
    : m_device{ device }
    , m_queue{ queue }
    , m_immediateCommandPool{ prev::util::vk::CreateCommandPool(m_device, m_queue.family) }
    , m_immediateCommandBuffer{ prev::util::vk::CreateCommandBuffer(m_device, m_immediateCommandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY) }
    , m_fence{ prev::util::vk::CreateFence(m_device) }
{
}

CommandsExecutor::~CommandsExecutor()
{
    vkDestroyFence(m_device, m_fence, VK_NULL_HANDLE);
    vkFreeCommandBuffers(m_device, m_immediateCommandPool, 1, &m_immediateCommandBuffer);
    vkDestroyCommandPool(m_device, m_immediateCommandPool, VK_NULL_HANDLE);
}

void CommandsExecutor::ExecuteImmediate(const std::function<void(VkCommandBuffer)>& func)
{
    VkCommandBufferBeginInfo cmdBufBeginInfo{ prev::util::vk::CreateStruct<VkCommandBufferBeginInfo>(VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO) };
    cmdBufBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VKERRCHECK(vkBeginCommandBuffer(m_immediateCommandBuffer, &cmdBufBeginInfo));

    func(m_immediateCommandBuffer);

    VKERRCHECK(vkEndCommandBuffer(m_immediateCommandBuffer));

    vkResetFences(m_device, 1, &m_fence);

    VkSubmitInfo submitInfo{ prev::util::vk::CreateStruct<VkSubmitInfo>(VK_STRUCTURE_TYPE_SUBMIT_INFO) };
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_immediateCommandBuffer;
    VKERRCHECK(m_queue.Submit(1, &submitInfo, m_fence));

    VKERRCHECK(vkWaitForFences(m_device, 1, &m_fence, VK_TRUE, UINT64_MAX));
}

} // namespace prev::core