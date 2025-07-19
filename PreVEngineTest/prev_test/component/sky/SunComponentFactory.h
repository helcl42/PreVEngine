#ifndef __SUN_COMPONENT_FACTORY_H__
#define __SUN_COMPONENT_FACTORY_H__

#include "ISunComponent.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>

namespace prev_test::component::sky {
class SunComponentFactory final {
public:
    SunComponentFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator);

    ~SunComponentFactory() = default;

public:
    std::unique_ptr<ISunComponent> Create() const;

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;
};
} // namespace prev_test::component::sky

#endif // !__SUN_COMPONENT_FACTORY_H__
