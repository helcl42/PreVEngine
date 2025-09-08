#include "DeviceExtensions.h"

namespace prev::core::device {
namespace {
    std::vector<std::string> GetItemNames(const VkPhysicalDevice gpu, const char* layerName)
    {
        uint32_t count{ 0 };
        VKERRCHECK(vkEnumerateDeviceExtensionProperties(gpu, layerName, &count, nullptr));
        std::vector<VkExtensionProperties> items(count);
        VKERRCHECK(vkEnumerateDeviceExtensionProperties(gpu, layerName, &count, items.data()));

        std::vector<std::string> itemNames(items.size());
        for (size_t i = 0; i < items.size(); ++i) {
            itemNames[i] = items[i].extensionName;
        }
        return itemNames;
    }
} // namespace

DeviceExtensions::DeviceExtensions(const VkPhysicalDevice gpu, const char* layerName)
    : PickList("DeviceExtensions", GetItemNames(gpu, layerName))
{
}
} // namespace prev::core::device