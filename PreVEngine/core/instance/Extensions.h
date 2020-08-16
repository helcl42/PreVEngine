#ifndef __EXTENSIONS_H__
#define __EXTENSIONS_H__

#include "../../common/PickList.h"
#include "Validation.h"

namespace prev {
class Extensions final : public prev::common::PickList {
public:
    Extensions(const char* layerName = nullptr);

public:
    const char* GetNameByIndex(uint32_t inx) const override;

    uint32_t GetPickedCount() const override;

    std::string GetName() const override;

private:
    std::vector<VkExtensionProperties> m_itemList;
};
} // namespace prev

#endif