#include "DeviceExtensions.h"

namespace prev::core::device {
DeviceExtensions::DeviceExtensions(const VkPhysicalDevice gpu, const char* layerName)
{
    uint32_t count{ 0 };
    VKERRCHECK(vkEnumerateDeviceExtensionProperties(gpu, layerName, &count, nullptr));

    m_itemList.resize(count);
    VKERRCHECK(vkEnumerateDeviceExtensionProperties(gpu, layerName, &count, m_itemList.data()));
}

DeviceExtensions::DeviceExtensions(const DeviceExtensions& other)
    : PickList(other)
{
    Refresh();
}

DeviceExtensions& DeviceExtensions::operator=(const DeviceExtensions& other)
{
    PickList::operator=(other);
    Refresh();
    return *this;
}

std::string DeviceExtensions::GetNameByIndex(const uint32_t index) const
{
    return m_itemList[index].extensionName;
}

std::string DeviceExtensions::GetName() const
{
    return "DeviceExtensions";
}
} // namespace prev::core::device