#ifndef __LAYERS_H__
#define __LAYERS_H__

#include "../../common/PickList.h"
#include "Validation.h"

namespace prev::core::instance {
class Layers final : public prev::common::PickList<VkLayerProperties> {
public:
    Layers();

public:
    std::string GetNameByIndex(const uint32_t index) const override;

    std::string GetName() const override;
};
} // namespace prev::core::instance

#endif