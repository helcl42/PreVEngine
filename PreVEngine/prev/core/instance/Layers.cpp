#include "Layers.h"

namespace prev::core::instance {
namespace {
    std::vector<std::string> GetItemNames()
    {
        uint32_t count{ 0 };
        VKERRCHECK(vkEnumerateInstanceLayerProperties(&count, nullptr));
        std::vector<VkLayerProperties> items(count);
        VKERRCHECK(vkEnumerateInstanceLayerProperties(&count, items.data()));

        std::vector<std::string> itemNames(items.size());
        for (size_t i = 0; i < items.size(); ++i) {
            itemNames[i] = items[i].layerName;
        }
        return itemNames;
    }
} // namespace

Layers::Layers()
    : PickList("Layers", GetItemNames())
{
}
} // namespace prev::core::instance