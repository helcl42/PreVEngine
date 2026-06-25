#ifndef __WATER_COMPONENT_FACTORY_H__
#define __WATER_COMPONENT_FACTORY_H__

#include "IWaterComponent.h"

#include <prev/core/device/Device.h>

namespace prev_test::component::water {
class WaterComponentFactory final {
public:
    WaterComponentFactory(prev::core::device::Device& device, bool async = true);

    ~WaterComponentFactory() = default;

public:
    std::unique_ptr<IWaterComponent> Create(const int x, const int z) const;

private:
    prev::core::device::Device& m_device;

    bool m_async{ true };
};
} // namespace prev_test::component::water

#endif
