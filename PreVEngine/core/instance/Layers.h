#ifndef __LAYERS_H__
#define __LAYERS_H__

#include "../../common/PickList.h"
#include "Validation.h"

namespace prev {
class Layers final : public PickList {
public:
    Layers();

public:
    const char* GetNameByIndex(uint32_t inx) const override;

    uint32_t GetPickedCount() const override;

    std::string GetName() const override;

private:
    std::vector<VkLayerProperties> m_itemList;
};
} // namespace prev

#endif