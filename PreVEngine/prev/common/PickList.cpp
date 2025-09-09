#include "PickList.h"

namespace prev::common {
PickList::PickList(const std::string& name, const std::vector<std::string>& names)
    : m_name{ name }
    , m_pickListNames{ names }
{
}

PickList::PickList(const PickList& other)
    : m_pickListIndices{ other.m_pickListIndices }
    , m_pickListNames{ other.m_pickListNames }
{
    Refresh();
}

PickList& PickList::operator=(const PickList& rhs)
{
    this->m_pickListIndices = rhs.m_pickListIndices;
    this->m_pickListNames = rhs.m_pickListNames;
    Refresh();
    return *this;
}

PickList::PickList(PickList&& other)
    : m_pickListIndices{ other.m_pickListIndices }
    , m_pickListNames{ other.m_pickListNames }
{
    Refresh();
    other = {};
}

PickList& PickList::operator=(PickList&& rhs)
{
    this->m_pickListIndices = rhs.m_pickListIndices;
    this->m_pickListNames = rhs.m_pickListNames;
    Refresh();
    rhs = {};
    return *this;
}

bool PickList::Pick(const std::vector<std::string>& list)
{
    bool found{ true };
    for (const auto& item : list) {
        found &= Pick(item);
    }
    return found;
}

bool PickList::Pick(const std::string& name)
{
    const auto index{ IndexOf(name) };
    if (index < 0) {
        LOGW("%s not found.", name.c_str()); // Warn if picked item was not found.
        return false;
    }
    return Pick(index);
}

bool PickList::Pick(const uint32_t index)
{
    if (index >= GetCount()) {
        return false; // Return false if index is out of range.
    }

    for (const auto pickItem : m_pickListIndices) {
        if (pickItem == index) {
            return true; // Check if item was already picked
        }
    }

    m_pickListIndices.push_back(index); // if not, add item to pick-list
    Refresh();
    return true;
}

bool PickList::UnPick(const std::string& name)
{
    bool found{ false };
    for (uint32_t i = 0; i < GetPickCount(); ++i) {
        const auto& nameAtIndex{ m_pickListNames[i] };
        if (nameAtIndex == name) {
            m_pickListIndices.erase(m_pickListIndices.begin() + i);
            found = true;
        }
    }

    Refresh();
    return found;
}

void PickList::PickAll()
{
    for (uint32_t i = 0; i < GetPickCount(); ++i) {
        Pick(i);
    }
}

void PickList::Clear()
{
    m_pickListIndices.clear();
    Refresh();
}

bool PickList::IsPicked(const std::string& name) const
{
    for (const auto index : m_pickListIndices) {
        const auto& nameAtIndex{ m_pickListNames[index] };
        if (nameAtIndex == name) {
            return true;
        }
    }
    return false;
}

const char* const* PickList::GetPickListRaw() const
{
    return m_pickListNamesPtrs.data();
}

const std::vector<std::string>& PickList::GetPickList() const
{
    return m_pickListNames;
}

uint32_t PickList::GetCount() const
{
    return static_cast<uint32_t>(m_pickListNames.size());
}

uint32_t PickList::GetPickCount() const
{
    return static_cast<uint32_t>(m_pickListIndices.size());
}

bool PickList::Contains(const std::string& name) const
{
    return IndexOf(name) >= 0;
}

void PickList::Print() const
{
    LOGI("%s picked: %u of %u", m_name.c_str(), GetPickCount(), GetCount());
    for (uint32_t i = 0; i < GetCount(); ++i) {
        const auto& name{ m_pickListNames[i] };
        const auto picked{ IsPicked(name) };
        if (picked) {
            print(ConsoleColor::RESET, "\t%s %s\n", TICK_CHARACTER, name.c_str());
        } else {
            print(ConsoleColor::FAINT, "\t%s %s\n", " ", name.c_str());
        }
    }
}

int32_t PickList::IndexOf(const std::string& name) const
{
    for (uint32_t i = 0; i < GetCount(); ++i) {
        const auto& nameAtIndex{ m_pickListNames[i] };
        if (nameAtIndex == name) {
            return i;
        }
    }
    return -1;
}

void PickList::Refresh()
{
    m_pickListNamesPtrs.resize(m_pickListIndices.size());

    for (size_t i = 0; i < m_pickListIndices.size(); ++i) {
        const auto index{ m_pickListIndices[i] };
        const auto& nameAtIndex{ m_pickListNames[index] };
        m_pickListNamesPtrs[i] = nameAtIndex.c_str();
    }
}
} // namespace prev::common