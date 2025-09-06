#include "Device.h"

#include "../../common/Logger.h"
#include "../../util/VkUtils.h"

#include <algorithm>
#include <iterator>
#include <stdexcept>

namespace prev::core::device {
Device::Device(const PhysicalDevice& gpu, const VkDevice handle, std::map<QueueType, std::vector<std::unique_ptr<Queue>>>&& queues)
    : m_gpu{ gpu }
    , m_handle{ handle }
    , m_queues{ std::move(queues) }
{
    LOGI("Logical Device created based on GPU: %s", m_gpu.GetProperties().deviceName);
    m_gpu.GetExtensions().Print();
}

Device::~Device()
{
    vkDeviceWaitIdle(m_handle);
    vkDestroyDevice(m_handle, nullptr);

    LOGI("Logical device destroyed");
}

void Device::WaitIdle() const
{
    vkDeviceWaitIdle(m_handle);
}

bool Device::HasQueue(const QueueType queueType, const uint32_t index) const
{
    const auto queuesIter{ m_queues.find(queueType) };
    if (queuesIter == m_queues.cend()) {
        LOGW("Trying to retrieve non existing QueueType.");
        return false;
    }

    const auto& queuesGroup{ queuesIter->second };
    if (index >= queuesGroup.size()) {
        LOGW("Trying access queue at invalid index %ud.", index);
        return false;
    }
    return true;
}

const Queue& Device::GetQueue(const QueueType queueType, const uint32_t index) const
{
    const auto queuesIter{ m_queues.find(queueType) };
    if (queuesIter == m_queues.cend()) {
        throw std::runtime_error("Trying to retrieve non existing QueueType.");
    }

    const auto& queuesGroup{ queuesIter->second };
    if (index >= queuesGroup.size()) {
        throw std::runtime_error("Trying access queue at invalid index " + std::to_string(index) + ".");
    }
    return *queuesGroup[index];
}

std::vector<QueueType> Device::GetAllQueueTypes() const
{
    std::vector<QueueType> result;
    std::transform(m_queues.begin(), m_queues.end(), std::back_inserter(result), [](const auto& pair) { return pair.first; });
    return result;
}

uint32_t Device::GetQueueTypeCount(const QueueType queueType) const
{
    const auto queuesIter{ m_queues.find(queueType) };
    if (queuesIter == m_queues.cend()) {
        return 0;
    }
    return static_cast<uint32_t>(queuesIter->second.size());
}

const PhysicalDevice& Device::GetGPU() const
{
    return m_gpu;
}

void Device::Print() const
{
    auto queueTypeToString = [](const QueueType type) {
        static const std::map<QueueType, std::string> queueNames = {
            { QueueType::PRESENT, "PRESENT" },
            { QueueType::GRAPHICS, "GRAPHICS" },
            { QueueType::COMPUTE, "COMPUTE" },
            { QueueType::TRANSFER, "TRANSFER" },
            { QueueType::SPARSE, "SPARSE" },
            { QueueType::PROTECTED, "PROTECTED" }
        };
        return queueNames.at(type);
    };

    auto canPresentToString = [](const VkSurfaceKHR surface) -> std::string {
        return surface ? "(can present)" : "";
    };

    LOGI("Logical Device used queues:");
    for (const auto& [qGroupKey, gQroupList] : m_queues) {
        for (const auto& qGroupItem : gQroupList) {
            LOGI("Queue purpose: %s family: %d index: %d flags: [ %s] %s", queueTypeToString(qGroupKey).c_str(), qGroupItem->family, qGroupItem->index, prev::util::vk::QueueFlagsToString(qGroupItem->flags).c_str(), canPresentToString(qGroupItem->surface).c_str());
        }
    }
}

Device::operator VkDevice() const
{
    return m_handle;
}
} // namespace prev::core::device
