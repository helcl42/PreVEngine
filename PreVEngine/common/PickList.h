#ifndef __PICK_LIST_H__
#define __PICK_LIST_H__

#include <assert.h>
#include <string>
#include <vector>

namespace PreVEngine {
class PickList {
public:
    PickList() = default;

    virtual ~PickList() = default;

protected:
    void Refresh();

public:
    virtual const char* GetNameByIndex(uint32_t inx) const = 0;

    virtual uint32_t GetPickedCount() const = 0;

    virtual std::string GetName() const = 0;

public:
    bool Pick(const std::vector<const char*>& list);

    bool Pick(const char* name);

    bool Pick(const uint32_t inx);

    void UnPick(const char* name);

    void PickAll();

    void Clear();

    int IndexOf(const char* name) const;

    bool IsPicked(const char* name) const;

    const char* const* GetPickList() const;

    uint32_t PickCount() const;

    void Print() const;

protected:
    std::vector<uint32_t> m_pickListIndices;

    std::vector<std::string> m_pickListNames;

    std::vector<const char*> m_pickListNamesPtrs;
};
} // namespace PreVEngine
#endif