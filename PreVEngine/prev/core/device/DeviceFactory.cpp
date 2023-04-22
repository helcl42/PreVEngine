#include "DeviceFactory.h"
#include "Device.h"
#include "Queue.h"

namespace prev::core::device {
std::shared_ptr<Device> DeviceFactory::Create(const std::shared_ptr<PhysicalDevice>& gpu, VkSurfaceKHR surface) const
{
    QueueMetadataStorage queuesMetadataStorage{};

    QueueMetadata workingQueue{};
    if (FindQueue(gpu, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT, nullptr, queuesMetadataStorage.GetAllQueues(), workingQueue)) { // there is dedicated compute queue
        queuesMetadataStorage.Add({ QueueType::COMPUTE }, { workingQueue });
    }

    if (queuesMetadataStorage.HasAny(QueueType::COMPUTE)) {
        if (FindQueue(gpu, VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT, surface, queuesMetadataStorage.GetAllQueues(), workingQueue)) {
            queuesMetadataStorage.Add({ QueueType::PRESENT, QueueType::GRAPHICS }, { workingQueue });
        }
    } else {
        if (FindQueue(gpu, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, 0, surface, queuesMetadataStorage.GetAllQueues(), workingQueue)) {
            queuesMetadataStorage.Add({ QueueType::PRESENT, QueueType::GRAPHICS, QueueType::COMPUTE }, { workingQueue });
        }
    }

    if (!queuesMetadataStorage.HasAny(QueueType::PRESENT)) {
        if (FindQueue(gpu, VK_QUEUE_GRAPHICS_BIT, 0, surface, queuesMetadataStorage.GetAllQueues(), workingQueue)) {
            queuesMetadataStorage.Add({ QueueType::PRESENT, QueueType::GRAPHICS }, { workingQueue });
        }
    }

    if (!queuesMetadataStorage.HasAny(QueueType::PRESENT)) {
        if (FindQueue(gpu, 0, 0, surface, queuesMetadataStorage.GetAllQueues(), workingQueue)) {
            queuesMetadataStorage.Add({ QueueType::PRESENT }, { workingQueue });
        }

        if (FindQueue(gpu, VK_QUEUE_GRAPHICS_BIT, 0, nullptr, queuesMetadataStorage.GetAllQueues(), workingQueue)) {
            queuesMetadataStorage.Add({ QueueType::GRAPHICS }, { workingQueue });
        }
    }

    if (!queuesMetadataStorage.HasAny(QueueType::GRAPHICS) || !queuesMetadataStorage.HasAny(QueueType::PRESENT) || !queuesMetadataStorage.HasAny(QueueType::COMPUTE)) {
        LOGE("Could not find all necessary queues.\n");
        return nullptr;
    }

    return std::make_shared<Device>(gpu, queuesMetadataStorage);
}

uint32_t DeviceFactory::FamilyQueueCount(const std::vector<QueueMetadata>& queues, const uint32_t family) const
{
    uint32_t count{ 0 };
    for (const auto& q : queues) {
        if (q.family == family) {
            count++;
        }
    }
    return count;
}

bool DeviceFactory::FindQueue(const std::shared_ptr<PhysicalDevice>& gpu, VkQueueFlags flags, VkQueueFlags unwantedFlags, VkSurfaceKHR surface, const std::vector<QueueMetadata>& addedQueues, QueueMetadata& outQueueMeta) const
{
    auto familyIndex{ gpu->FindQueueFamily(flags, unwantedFlags, surface) };
    if (familyIndex < 0) {
        LOGW("Could not find queueFamily with requested properties.\n");
        return false;
    }

    const auto queueFamily{ gpu->GetQueueFamilies().at(familyIndex) };
    uint32_t queueIndex{ FamilyQueueCount(addedQueues, familyIndex) };
    if (queueIndex == queueFamily.queueCount) {
        LOGW("No more queues available from this family.\n");
        return false;
    }

    LOGI("Queue: %d  flags: [ %s%s%s%s%s]%s\n", queueIndex, (queueFamily.queueFlags & 1) ? "GRAPHICS " : "", (queueFamily.queueFlags & 2) ? "COMPUTE " : "", (queueFamily.queueFlags & 4) ? "TRANSFER " : "", (queueFamily.queueFlags & 8) ? "SPARSE " : "", (queueFamily.queueFlags & 16) ? "PROTECTED" : "", surface ? " (can present)" : "");

    outQueueMeta = QueueMetadata{ static_cast<uint32_t>(familyIndex), queueIndex, flags, surface };
    return true;
}
} // namespace prev::core::device