#include "Extensions.h"

namespace prev::core::instance {
Extensions::Extensions(const char* layerName)
{
    uint32_t count{ 0 };
    VKERRCHECK(vkEnumerateInstanceExtensionProperties(layerName, &count, nullptr)); // Get list size

    m_itemList.resize(count); // Resize buffer
    VKERRCHECK(vkEnumerateInstanceExtensionProperties(layerName, &count, m_itemList.data())); // Fetch list
}

std::string Extensions::GetNameByIndex(const uint32_t index) const
{
    return m_itemList[index].extensionName;
}

std::string Extensions::GetName() const
{
    return "Extensions";
}
} // namespace prev::core::instance