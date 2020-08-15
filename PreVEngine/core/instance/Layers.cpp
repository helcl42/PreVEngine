#include "Layers.h"

namespace prev {
Layers::Layers()
{
    uint32_t count = 0;
    VKERRCHECK(vkEnumerateInstanceLayerProperties(&count, nullptr));

    m_itemList.resize(count);
    VKERRCHECK(vkEnumerateInstanceLayerProperties(&count, m_itemList.data()));
}

const char* Layers::GetNameByIndex(uint32_t inx) const
{
    return static_cast<const char*>(m_itemList.at(inx).layerName);
}

uint32_t Layers::GetPickedCount() const
{
    return static_cast<uint32_t>(m_itemList.size());
}

std::string Layers::GetName() const
{
    return "Layers";
}
} // namespace prev