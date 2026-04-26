#ifndef __SUN_COMPONENT_FACTORY_H__
#define __SUN_COMPONENT_FACTORY_H__

#include "ISunComponent.h"

#include <prev/core/device/Device.h>

namespace prev_test::component::sky {
class SunComponentFactory final {
public:
    SunComponentFactory(prev::core::device::Device& device);

    ~SunComponentFactory() = default;

public:
    std::unique_ptr<ISunComponent> Create() const;

private:
    prev::core::device::Device& m_device;

};
} // namespace prev_test::component::sky

#endif // !__SUN_COMPONENT_FACTORY_H__
