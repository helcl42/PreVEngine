#include "DeviceFactory.h"
#include "Queue.h"

#include "../../common/Logger.h"
#include "../../util/MathUtils.h"

#include <stdexcept>
#include <vector>

namespace prev::core::device {
std::unique_ptr<Device> DeviceFactory::Create(const PhysicalDevice& gpu, const std::vector<std::string>& extensions) const
{
    std::vector<const char*> extPtrs;
    extPtrs.reserve(extensions.size());
    for (const auto& e : extensions) {
        extPtrs.push_back(e.c_str());
    }

    GfxDeviceDescriptor deviceDesc{};
    deviceDesc.sType = GFX_STRUCTURE_TYPE_DEVICE_DESCRIPTOR;
    deviceDesc.pNext = nullptr;
    deviceDesc.label = "Main Device";
    deviceDesc.queueRequests = nullptr;
    deviceDesc.queueRequestCount = 0;
    deviceDesc.enabledExtensions = extPtrs.empty() ? nullptr : extPtrs.data();
    deviceDesc.enabledExtensionCount = static_cast<uint32_t>(extPtrs.size());

    GfxDevice gfxDevice{};
    if (gfxAdapterCreateDevice(gpu, &deviceDesc, &gfxDevice) != GFX_RESULT_SUCCESS) {
        throw std::runtime_error("Failed to create gfx device");
    }

    GfxQueue gfxQueue{};
    if (gfxDeviceGetQueue(gfxDevice, &gfxQueue) != GFX_RESULT_SUCCESS) {
        gfxDeviceDestroy(gfxDevice);
        throw std::runtime_error("Failed to get device queue");
    }

    // Determine the real family index and its actual capability flags from the adapter.
    // gfx defaults to the graphics queue family when no explicit queue requests are made.
    const auto queueFamilies{ gpu.GetQueueFamilies() };
    const int32_t familyIndex{ gpu.FindQueueFamily(GFX_QUEUE_FLAG_GRAPHICS) };
    if (familyIndex < 0 || static_cast<uint32_t>(familyIndex) >= static_cast<uint32_t>(queueFamilies.size())) {
        gfxDeviceDestroy(gfxDevice);
        throw std::runtime_error("Failed to find graphics queue family");
    }
    const GfxQueueFlags actualFlags{ queueFamilies[static_cast<uint32_t>(familyIndex)].flags };

    auto makeQueue = [&]() { return std::make_unique<Queue>(gfxQueue, static_cast<uint32_t>(familyIndex), 0, actualFlags); };

    std::map<QueueType, std::vector<std::unique_ptr<Queue>>> queues;
    // Present shares the graphics family (PhysicalDevices::Find guarantees surface support on it).
    queues[QueueType::PRESENT].push_back(makeQueue());
    if (prev::util::math::HasAnyFlagsSet(actualFlags, static_cast<GfxQueueFlags>(GFX_QUEUE_FLAG_GRAPHICS))) {
        queues[QueueType::GRAPHICS].push_back(makeQueue());
    }
    if (prev::util::math::HasAnyFlagsSet(actualFlags, static_cast<GfxQueueFlags>(GFX_QUEUE_FLAG_COMPUTE))) {
        queues[QueueType::COMPUTE].push_back(makeQueue());
    }
    if (prev::util::math::HasAnyFlagsSet(actualFlags, static_cast<GfxQueueFlags>(GFX_QUEUE_FLAG_TRANSFER))) {
        queues[QueueType::TRANSFER].push_back(makeQueue());
    }

    LOGI("GFX device created");
    return std::make_unique<Device>(gpu, gfxDevice, std::move(queues));
}
} // namespace prev::core::device
