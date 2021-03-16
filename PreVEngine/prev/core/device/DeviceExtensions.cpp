#include "DeviceExtensions.h"

namespace prev::core::device {
DeviceExtensions& DeviceExtensions::operator=(const DeviceExtensions& other)
{
    if (this != &other) {
        this->m_itemList = other.m_itemList;
        this->m_pickListIndices = other.m_pickListIndices;
        Refresh();
    }
    return *this;
}

DeviceExtensions::DeviceExtensions(const DeviceExtensions& other)
{
    this->m_itemList = other.m_itemList;
    this->m_pickListIndices = other.m_pickListIndices;
    Refresh();
}

void DeviceExtensions::Init(VkPhysicalDevice phy, const char* layerName)
{
    uint32_t count{ 0 };
    VKERRCHECK(vkEnumerateDeviceExtensionProperties(phy, layerName, &count, nullptr));

    m_itemList.resize(count); // Resize buffer
    VKERRCHECK(vkEnumerateDeviceExtensionProperties(phy, layerName, &count, m_itemList.data())); // Fetch list
}

const char* DeviceExtensions::GetNameByIndex(uint32_t inx) const
{
    return static_cast<const char*>(m_itemList.at(inx).extensionName);
}

uint32_t DeviceExtensions::GetPickedCount() const
{
    return static_cast<uint32_t>(m_itemList.size());
}

std::string DeviceExtensions::GetName() const
{
    return "Device-Extensions";
}
} // namespace prev::core::device