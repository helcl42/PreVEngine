#ifndef __SKY_COMPONENT_FACTORY_H__
#define __SKY_COMPONENT_FACTORY_H__

#include "ISkyComponent.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>

namespace prev_test::component::sky {
class SkyComponentFactory final {
public:
    SkyComponentFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator);

    ~SkyComponentFactory() = default;

public:
    std::unique_ptr<ISkyComponent> Create() const;

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;
};
} // namespace prev_test::component::sky

#endif // !__SKY_COMPONENT_FACTORY_H__
