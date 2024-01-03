#ifndef __WATER_COMPONENT_FACTORY_H__
#define __WATER_COMPONENT_FACTORY_H__

#include "IWaterComponent.h"

namespace prev_test::component::water {
class WaterComponentFactory final {
public:
    std::unique_ptr<IWaterComponent> Create(const int x, const int z) const;
};
} // namespace prev_test::component::water

#endif
