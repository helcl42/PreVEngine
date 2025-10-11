#include "DeviceFactory.h"
#include "Device.h"
#include "Queue.h"

#include "../../common/Logger.h"
#include "../../util/VkUtils.h"

#include <set>

static constexpr bool QUERY_DEDICATED_PRESENT_QUEUE{ false };
static constexpr bool QUERY_DEDICATED_COMPUTE_QUEUE{ true };

namespace prev::core::device {
namespace {
    struct QueueMetadata {
        uint32_t family{}; // queue family

        uint32_t index{}; // queue index

        VkQueueFlags flags{}; // Graphics / Compute / Transfer / Sparse / Protected

        VkSurfaceKHR surface{}; // VK_NULL_HANDLE if queue can not present

        friend bool operator<(const QueueMetadata& a, const QueueMetadata& b)
        {
            if (a.family >= b.family) {
                return false;
            }
            if (a.index >= b.index) {
                return false;
            }
            return true;
        }
    };

    struct QueuesMetadata {
        std::map<QueueType, std::set<QueueMetadata>> queueGroups;

        void Add(const std::vector<QueueType>& queueTypes, const QueueMetadata& queueMetadata)
        {
            for (const auto& queueType : queueTypes) {
                queueGroups[queueType].insert(queueMetadata);
            }
        }

        std::vector<uint32_t> GetQueueFamilies() const
        {
            std::set<uint32_t> distinctQueueFamilies;
            for (const auto& [queueType, queues] : queueGroups) {
                for (const auto& queue : queues) {
                    distinctQueueFamilies.insert(queue.family);
                }
            }
            return std::vector<uint32_t>(distinctQueueFamilies.cbegin(), distinctQueueFamilies.cend());
        }

        uint32_t GetQueueFamilyCount(const uint32_t family) const
        {
            std::set<uint32_t> distinctQueues;
            for (const auto& [queueType, queues] : queueGroups) {
                for (const auto& queue : queues) {
                    if (queue.family == family) {
                        distinctQueues.insert(queue.index);
                    }
                }
            }
            return static_cast<uint32_t>(distinctQueues.size());
        }

        std::vector<QueueMetadata> GetAllQueues() const
        {
            std::vector<QueueMetadata> result;
            for (const auto& group : queueGroups) {
                result.insert(result.end(), group.second.cbegin(), group.second.cend());
            }
            return result;
        }

        bool HasAny(const QueueType queueType) const
        {
            return queueGroups.find(queueType) != queueGroups.cend();
        }
    };

    bool FindQueue(const PhysicalDevice& gpu, const VkQueueFlags flags, const VkQueueFlags unwantedFlags, const VkSurfaceKHR surface, const QueuesMetadata& queuesMetadata, QueueMetadata& outQueueMeta)
    {
        const auto familyIndex{ gpu.FindQueueFamily(flags, unwantedFlags, surface) };
        if (familyIndex < 0) {
            LOGW("Could not find queueFamily with requested properties.");
            return false;
        }

        const auto& queueFamilyProps{ gpu.GetQueueFamilies().at(familyIndex) };

        uint32_t queueIndex{ 0 };
        const uint32_t usedFamilyQueueCount{ queuesMetadata.GetQueueFamilyCount(familyIndex) };
        if (usedFamilyQueueCount < queueFamilyProps.queueCount) {
            queueIndex = usedFamilyQueueCount;
        } else {
            LOGW("No more queues available from this family - fallback to the first one.");
        }

        outQueueMeta = QueueMetadata{ static_cast<uint32_t>(familyIndex), queueIndex, queueFamilyProps.queueFlags, surface };
        return true;
    }

    QueuesMetadata FindQueues(const PhysicalDevice& gpu, const VkSurfaceKHR surface)
    {
        QueuesMetadata queuesMetadata{};

        QueueMetadata workingQueueMetadata{};

        const uint32_t DefaultUnwantedFlags{ 0x20 | 0x40 }; // video decode/encode

        if constexpr (QUERY_DEDICATED_PRESENT_QUEUE) {
            // check if there is pure presenting queue with no other features
            if (FindQueue(gpu, 0, DefaultUnwantedFlags | VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT, surface, queuesMetadata, workingQueueMetadata)) {
                queuesMetadata.Add({ QueueType::PRESENT }, workingQueueMetadata);
            }

            if (!queuesMetadata.HasAny(QueueType::PRESENT)) {
                if (FindQueue(gpu, 0, DefaultUnwantedFlags, surface, queuesMetadata, workingQueueMetadata)) {
                    queuesMetadata.Add({ QueueType::PRESENT }, workingQueueMetadata);
                }
            }
        }

        if constexpr (QUERY_DEDICATED_COMPUTE_QUEUE) {
            // check if there is a dedicated compute queue without graphics
            if (FindQueue(gpu, VK_QUEUE_COMPUTE_BIT, DefaultUnwantedFlags | VK_QUEUE_GRAPHICS_BIT, nullptr, queuesMetadata, workingQueueMetadata)) { // there is a dedicated compute queue
                queuesMetadata.Add({ QueueType::COMPUTE }, workingQueueMetadata);
            }

            if (!queuesMetadata.HasAny(QueueType::COMPUTE)) {
                if (FindQueue(gpu, VK_QUEUE_COMPUTE_BIT, DefaultUnwantedFlags, nullptr, queuesMetadata, workingQueueMetadata)) {
                    queuesMetadata.Add({ QueueType::COMPUTE }, workingQueueMetadata);
                }
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
        return queuesMetadata;
    }

} // namespace

std::unique_ptr<Device> DeviceFactory::Create(const PhysicalDevice& gpu, const VkSurfaceKHR surface) const
{
    const QueuesMetadata queuesMetadata{ FindQueues(gpu, surface) };
    if ((surface && !queuesMetadata.HasAny(QueueType::PRESENT)) || !queuesMetadata.HasAny(QueueType::GRAPHICS) || !queuesMetadata.HasAny(QueueType::COMPUTE)) {
        LOGE("Could not find all necessary queues.");
        return nullptr;
    }

    const auto queueFamilyIndices{ queuesMetadata.GetQueueFamilies() };

    const float DefaultQueuePriority{ 1.0f };
    std::vector<std::vector<float>> allQueuesPriorities(queueFamilyIndices.size());
    for (size_t i = 0; i < queueFamilyIndices.size(); ++i) {
        const auto queueFamilyIndex{ queueFamilyIndices[i] };
        const auto queueCount{ queuesMetadata.GetQueueFamilyCount(queueFamilyIndex) };
        allQueuesPriorities[i] = std::vector<float>(queueCount, DefaultQueuePriority);
    }

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfoList;
    for (size_t i = 0; i < queueFamilyIndices.size(); ++i) {
        const auto queueFamilyIndex{ queueFamilyIndices[i] };
        const auto queueCount{ queuesMetadata.GetQueueFamilyCount(queueFamilyIndex) };
        const auto& priorities{ allQueuesPriorities[i] };

        VkDeviceQueueCreateInfo info{ prev::util::vk::CreateStruct<VkDeviceQueueCreateInfo>(VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO) };
        info.queueFamilyIndex = queueFamilyIndex;
        info.queueCount = queueCount;
        info.pQueuePriorities = priorities.data();
        queueCreateInfoList.push_back(info);
    }

    const auto& extensions{ gpu.GetExtensions() };
    const auto& features{ gpu.GetEnabledFeatures2() };

    VkDeviceCreateInfo deviceCreateInfo{ prev::util::vk::CreateStruct<VkDeviceCreateInfo>(VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO) };
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfoList.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfoList.data();
    deviceCreateInfo.enabledExtensionCount = extensions.GetPickCount();
    deviceCreateInfo.ppEnabledExtensionNames = extensions.GetPickListRaw();
    deviceCreateInfo.pEnabledFeatures = nullptr;
    deviceCreateInfo.pNext = &features;

    VkDevice device;
    VKERRCHECK(vkCreateDevice(gpu, &deviceCreateInfo, nullptr, &device)); // create device

#ifdef ENABLE_VK_LOADER
    volkLoadDevice(device);
#endif

    std::map<QueueType, std::vector<std::unique_ptr<Queue>>> queues;
    for (const auto& [queueGroupKey, queueGroupList] : queuesMetadata.queueGroups) {
        for (const auto& groupItem : queueGroupList) {
            VkQueue qHandle{};
            vkGetDeviceQueue(device, groupItem.family, groupItem.index, &qHandle);
            queues[queueGroupKey].emplace_back(std::make_unique<Queue>(qHandle, groupItem.family, groupItem.index, groupItem.flags, groupItem.surface));
        }
    }

    return std::make_unique<Device>(gpu, device, std::move(queues));
}
} // namespace prev::core::device