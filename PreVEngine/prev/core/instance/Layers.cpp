#include "Layers.h"

namespace prev::core::instance {
Layers::Layers()
{
    uint32_t count{ 0 };
    VKERRCHECK(vkEnumerateInstanceLayerProperties(&count, nullptr));

    m_itemList.resize(count);
    VKERRCHECK(vkEnumerateInstanceLayerProperties(&count, m_itemList.data()));
}

std::string Layers::GetNameByIndex(const uint32_t index) const
{
    return m_itemList[index].layerName;
}

std::string Layers::GetName() const
{
    return "Layers";
}
} // namespace prev::core::instance