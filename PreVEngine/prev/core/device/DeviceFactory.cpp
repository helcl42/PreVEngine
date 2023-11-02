#include "DeviceFactory.h"
#include "Device.h"
#include "Queue.h"

namespace prev::core::device {
namespace {
    bool FindQueue(const std::shared_ptr<PhysicalDevice>& gpu, const VkQueueFlags flags, const VkQueueFlags unwantedFlags, const VkSurfaceKHR surface, const QueuesMetadata& queuesMetadata, QueueMetadata& outQueueMeta)
    {
        const auto familyIndex{ gpu->FindQueueFamily(flags, unwantedFlags, surface) };
        if (familyIndex < 0) {
            LOGW("Could not find queueFamily with requested properties.\n");
            return false;
        }

        const auto& queueFamilyProps{ gpu->GetQueueFamilies().at(familyIndex) };

        uint32_t queueIndex{ 0 };
        const uint32_t usedFamilyQueueCount{ queuesMetadata.GetQueueFamilyCount(familyIndex) };
        if (usedFamilyQueueCount < queueFamilyProps.queueCount) {
            queueIndex = usedFamilyQueueCount;
        } else {
            LOGW("No more queues available from this family - fallback to to the first ones.\n");
        }

        outQueueMeta = QueueMetadata{ static_cast<uint32_t>(familyIndex), queueIndex, queueFamilyProps.queueFlags, surface };
        return true;
    }
} // namespace

std::shared_ptr<Device> DeviceFactory::Create(const std::shared_ptr<PhysicalDevice>& gpu, const VkSurfaceKHR surface) const
{
    QueuesMetadata queuesMetadata{};

    QueueMetadata workingQueueMetadata{};

    const uint32_t DefaultUnwantedFlags{ 0x20 | 0x40 }; // video decode/encode

    // check if there is pure presenting queue with no other features
    if (FindQueue(gpu, 0, DefaultUnwantedFlags | VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT, surface, queuesMetadata, workingQueueMetadata)) {
        queuesMetadata.Add({ QueueType::PRESENT }, workingQueueMetadata);
    }

    if (!queuesMetadata.HasAny(QueueType::PRESENT)) {
        if (FindQueue(gpu, 0, DefaultUnwantedFlags, surface, queuesMetadata, workingQueueMetadata)) {
            queuesMetadata.Add({ QueueType::PRESENT }, workingQueueMetadata);
        }
    }

    // check if there is a dedicated compute queue without graphics
    if (FindQueue(gpu, VK_QUEUE_COMPUTE_BIT, DefaultUnwantedFlags | VK_QUEUE_GRAPHICS_BIT, nullptr, queuesMetadata, workingQueueMetadata)) { // there is a dedicated compute queue
        queuesMetadata.Add({ QueueType::COMPUTE }, workingQueueMetadata);
    }

    if (!queuesMetadata.HasAny(QueueType::COMPUTE)) {
        if (FindQueue(gpu, VK_QUEUE_COMPUTE_BIT, DefaultUnwantedFlags, nullptr, queuesMetadata, workingQueueMetadata)) {
            queuesMetadata.Add({ QueueType::COMPUTE }, workingQueueMetadata);
        }
    }

    if (queuesMetadata.HasAny(QueueType::PRESENT)) {
        if (queuesMetadata.HasAny(QueueType::COMPUTE)) {
            if (FindQueue(gpu, VK_QUEUE_GRAPHICS_BIT, DefaultUnwantedFlags, nullptr, queuesMetadata, workingQueueMetadata)) {
                queuesMetadata.Add({ QueueType::GRAPHICS }, workingQueueMetadata);
            }
        } else {
            if (FindQueue(gpu, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, DefaultUnwantedFlags, nullptr, queuesMetadata, workingQueueMetadata)) {
                queuesMetadata.Add({ QueueType::GRAPHICS, QueueType::COMPUTE }, workingQueueMetadata);
            }
        }
    } else {
        if (queuesMetadata.HasAny(QueueType::COMPUTE)) {
            if (FindQueue(gpu, VK_QUEUE_GRAPHICS_BIT, DefaultUnwantedFlags, surface, queuesMetadata, workingQueueMetadata)) {
                queuesMetadata.Add({ QueueType::PRESENT, QueueType::GRAPHICS }, workingQueueMetadata);
            }
        } else {
            if (FindQueue(gpu, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, DefaultUnwantedFlags, surface, queuesMetadata, workingQueueMetadata)) {
                queuesMetadata.Add({ QueueType::PRESENT, QueueType::GRAPHICS, QueueType::COMPUTE }, workingQueueMetadata);
            }
        }
    }

    if (!queuesMetadata.HasAny(QueueType::PRESENT) || !queuesMetadata.HasAny(QueueType::GRAPHICS) || !queuesMetadata.HasAny(QueueType::COMPUTE)) {
        LOGE("Could not find all necessary queues.\n");
        return nullptr;
    }

    return std::make_shared<Device>(gpu, queuesMetadata);
}
} // namespace prev::core::device