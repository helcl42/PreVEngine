#ifndef __PICK_LIST_H__
#define __PICK_LIST_H__

#include <cinttypes>
#include <string>
#include <vector>

#include "Logger.h"

namespace prev::common {
template <typename ItemType>
class PickList {
public:
    PickList() = default;

    virtual ~PickList() = default;

    PickList(const PickList& other)
        : m_itemList{ other.m_itemList }
        , m_pickListIndices{ other.m_pickListIndices }
    {
    }

    PickList& operator=(const PickList& rhs)
    {
        this->m_itemList = rhs.m_itemList;
        this->m_pickListIndices = rhs.m_pickListIndices;
        return *this;
    }

public:
    virtual std::string GetNameByIndex(const uint32_t index) const = 0;

    virtual std::string GetName() const = 0;

public:
    bool Pick(const std::vector<std::string>& list)
    {
        bool found{ true };
        for (const auto& item : list) {
            found &= Pick(item);
        }
        return found;
    }

    bool Pick(const std::string& name)
    {
        const auto index{ IndexOf(name) };
        if (index < 0) {
            LOGW("%s not found.\n", name.c_str()); // Warn if picked item was not found.
            return false;
        }
        return Pick(index);
    }

    bool Pick(const uint32_t index)
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

    bool UnPick(const std::string& name)
    {
        bool found{ false };
        for (uint32_t i = 0; i < GetPickCount(); ++i) {
            const auto& nameAtIndex{ GetNameByIndex(m_pickListIndices[i]) };
            if (nameAtIndex == name) {
                m_pickListIndices.erase(m_pickListIndices.begin() + i);
                found = true;
            }
        }

        Refresh();
        return found;
    }

    void PickAll()
    {
        for (uint32_t i = 0; i < GetPickCount(); ++i) {
            Pick(i);
        }
    }

    void Clear()
    {
        m_pickListIndices.clear();
        m_pickListNames.clear();
        m_pickListNamesPtrs.clear();
    }

    bool IsPicked(const std::string& name) const
    {
        for (const auto index : m_pickListIndices) {
            const auto& nameAtIndex{ GetNameByIndex(index) };
            if (nameAtIndex == name) {
                return true;
            }
        }
        return false;
    }

    const char* const* GetPickListRaw() const
    {
        return m_pickListNamesPtrs.data();
    }

    const std::vector<std::string>& GetPickList() const
    {
        return m_pickListNames;
    }

    uint32_t GetCount() const
    {
        return static_cast<uint32_t>(m_itemList.size());
    }

    uint32_t GetPickCount() const
    {
        return static_cast<uint32_t>(m_pickListIndices.size());
    }

    bool Contains(const std::string& name) const
    {
        return IndexOf(name) >= 0;
    }

    void Print() const
    {
        LOGI("%s picked: %d of %d\n", GetName().c_str(), GetPickCount(), GetCount());
        for (uint32_t i = 0; i < GetCount(); ++i) {
            const auto& name{ GetNameByIndex(i) };
            const auto picked{ IsPicked(name) };
            if (picked) {
                print(ConsoleColor::RESET, "\t%s %s\n", TICK_CHARACTER, name.c_str());
            } else {
                print(ConsoleColor::FAINT, "\t%s %s\n", " ", name.c_str());
            }
        }
    }

protected:
    int32_t IndexOf(const std::string& name) const
    {
        for (uint32_t i = 0; i < GetCount(); ++i) {
            const auto& nameAtIndex{ GetNameByIndex(i) };
            if (nameAtIndex == name) {
                return i;
            }
        }
        return -1;
    }

    void Refresh()
    {
        m_pickListNames.resize(m_pickListIndices.size());
        m_pickListNamesPtrs.resize(m_pickListIndices.size());

        for (size_t i = 0; i < m_pickListIndices.size(); ++i) {
            const auto index{ m_pickListIndices[i] };
            const auto& nameAtIndex{ GetNameByIndex(index) };
            m_pickListNames[i] = nameAtIndex;
            m_pickListNamesPtrs[i] = m_pickListNames[i].c_str();
        }
    }

protected:
    std::vector<ItemType> m_itemList;

    std::vector<uint32_t> m_pickListIndices;

    std::vector<std::string> m_pickListNames;

    std::vector<const char*> m_pickListNamesPtrs;
};
} // namespace prev::common
#endif