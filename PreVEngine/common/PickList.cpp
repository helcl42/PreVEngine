#include "PickList.h"

#include "../core/instance/Validation.h"

#include <cstring>

namespace prev {
bool PickList::IsPicked(const char* name) const
{
    for (auto index : m_pickListIndices) {
        if (strcmp(name, GetNameByIndex(index)) == 0) {
            return true;
        }
    }
    return false;
}

int PickList::IndexOf(const char* name) const
{
    for (unsigned int i = 0; i < GetPickedCount(); i++) {
        if (strcmp(name, GetNameByIndex(i)) == 0) {
            return i;
        }
    }
    return -1;
}

bool PickList::Pick(const std::vector<const char*>& list)
{
    bool found = true;
    for (auto item : list) {
        found &= Pick(item);
    }
    return found;
}

bool PickList::Pick(const char* name)
{
    int inx = IndexOf(name);
    if (inx > -1) {
        return Pick(inx);
    }
    LOGW("%s not found.\n", name); // Warn if picked item was not found.
    return false;
}

bool PickList::Pick(const uint32_t inx)
{
    if (inx >= GetPickedCount()) {
        return false; // Return false if index is out of range.
    }

    for (const auto pickItem : m_pickListIndices) {
        if (pickItem == inx) {
            return true; // Check if item was already picked
        }
    }

    m_pickListIndices.push_back(inx); // if not, add item to pick-list

    Refresh();

    return true;
}

void PickList::UnPick(const char* name)
{
    for (uint32_t i = 0; i < PickCount(); i++) {
        if (strcmp(name, GetNameByIndex(m_pickListIndices[i])) == 0) {
            m_pickListIndices.erase(m_pickListIndices.begin() + i);
        }
    }

    Refresh();
}

void PickList::Refresh()
{
    m_pickListNames.resize(m_pickListIndices.size());
    m_pickListNamesPtrs.resize(m_pickListIndices.size());

    for (size_t i = 0; i < m_pickListIndices.size(); i++) {
        const char* name = GetNameByIndex(m_pickListIndices.at(i));
        m_pickListNames[i] = std::string(name);
        m_pickListNamesPtrs[i] = m_pickListNames.at(i).c_str();
    }
}

void PickList::PickAll()
{
    for (uint32_t i = 0; i < GetPickedCount(); i++) {
        Pick(i);
    }
}

void PickList::Clear()
{
    m_pickListIndices.clear();
    m_pickListNames.clear();
    m_pickListNamesPtrs.clear();
}

const char* const* PickList::GetPickList() const
{
    return static_cast<const char* const*>(m_pickListNamesPtrs.data());
}

uint32_t PickList::PickCount() const
{
    return static_cast<uint32_t>(m_pickListIndices.size());
}

void PickList::Print() const
{
    printf("%s picked: %d of %d\n", GetName().c_str(), PickCount(), GetPickedCount());
    for (uint32_t i = 0; i < GetPickedCount(); i++) {
        bool picked = false;
        const char* name = GetNameByIndex(i);
        for (const auto pickIndex : m_pickListIndices) {
            picked |= (strcmp(GetNameByIndex(pickIndex), name) == 0);
        }

        if (picked) {
            print(ConsoleColor::RESET, "\t%s %s\n", cTICK, name);
        } else {
            print(ConsoleColor::FAINT, "\t%s %s\n", " ", name);
        }
    }
}
} // namespace prev