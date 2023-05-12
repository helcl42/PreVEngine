#include "Extensions.h"

namespace prev::core::instance {
Extensions::Extensions(const char* layerName)
{
    uint32_t count = 0;
    VKERRCHECK(vkEnumerateInstanceExtensionProperties(layerName, &count, nullptr)); // Get list size

    m_itemList.resize(count); // Resize buffer
    VKERRCHECK(vkEnumerateInstanceExtensionProperties(layerName, &count, m_itemList.data())); // Fetch list
}

const char* Extensions::GetNameByIndex(uint32_t inx) const
{
    return static_cast<const char*>(m_itemList.at(inx).extensionName);
}

uint32_t Extensions::GetPickedCount() const
{
    return static_cast<uint32_t>(m_itemList.size());
}

std::string Extensions::GetName() const
{
    return "Extensions";
}
} // namespace prev::core::instance