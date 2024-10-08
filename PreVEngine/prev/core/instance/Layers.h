#ifndef __LAYERS_H__
#define __LAYERS_H__

#include "../../common/PickList.h"

#include "../Core.h"

namespace prev::core::instance {
class Layers final : public prev::common::PickList<VkLayerProperties> {
public:
    Layers();

    Layers(const Layers& other);

    Layers& operator=(const Layers& other);

public:
    std::string GetNameByIndex(const uint32_t index) const override;

    std::string GetName() const override;
};
} // namespace prev::core::instance

#endif