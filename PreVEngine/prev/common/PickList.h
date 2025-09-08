#ifndef __PICK_LIST_H__
#define __PICK_LIST_H__

#include <cinttypes>
#include <string>
#include <vector>

#include "Logger.h"

namespace prev::common {
class PickList {
public:
    PickList() = default;

    PickList(const std::string& name, const std::vector<std::string>& pickListNames);

    PickList(const PickList& other);

    PickList& operator=(const PickList& rhs);

    PickList(PickList&& other);

    PickList& operator=(PickList&& rhs);

    ~PickList() = default;

public:
    bool Pick(const std::vector<std::string>& list);

    bool Pick(const std::string& name);

    bool Pick(const uint32_t index);

    bool UnPick(const std::string& name);

    void PickAll();

    void Clear();

    bool IsPicked(const std::string& name) const;

    const char* const* GetPickListRaw() const;

    const std::vector<std::string>& GetPickList() const;

    uint32_t GetCount() const;

    uint32_t GetPickCount() const;

    bool Contains(const std::string& name) const;

    void Print() const;

protected:
    int32_t IndexOf(const std::string& name) const;

    void Refresh();

protected:
    std::string m_name{};

    std::vector<uint32_t> m_pickListIndices;

    std::vector<std::string> m_pickListNames;

    std::vector<const char*> m_pickListNamesPtrs;
};
} // namespace prev::common
#endif