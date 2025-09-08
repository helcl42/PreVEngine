#include "Extensions.h"

namespace prev::core::instance {
namespace {
    std::vector<std::string> GetItemNames(const char* layerName)
    {
        uint32_t count{ 0 };
        VKERRCHECK(vkEnumerateInstanceExtensionProperties(layerName, &count, nullptr));
        std::vector<VkExtensionProperties> items(count);
        VKERRCHECK(vkEnumerateInstanceExtensionProperties(layerName, &count, items.data()));

        std::vector<std::string> itemNames(items.size());
        for (size_t i = 0; i < items.size(); ++i) {
            itemNames[i] = items[i].extensionName;
        }
        return itemNames;
    }
} // namespace

Extensions::Extensions(const char* layerName)
    : PickList("Extensions", GetItemNames(layerName))
{
}
} // namespace prev::core::instance