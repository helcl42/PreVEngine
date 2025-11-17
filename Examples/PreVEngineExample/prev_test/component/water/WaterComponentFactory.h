#ifndef __WATER_COMPONENT_FACTORY_H__
#define __WATER_COMPONENT_FACTORY_H__

#include "IWaterComponent.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>

namespace prev_test::component::water {
class WaterComponentFactory final {
public:
    WaterComponentFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator);

    ~WaterComponentFactory() = default;

public:
    std::unique_ptr<IWaterComponent> Create(const int x, const int z) const;

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;
};
} // namespace prev_test::component::water

#endif
