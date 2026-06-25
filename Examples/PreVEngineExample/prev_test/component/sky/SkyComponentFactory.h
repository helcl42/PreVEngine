#ifndef __SKY_COMPONENT_FACTORY_H__
#define __SKY_COMPONENT_FACTORY_H__

#include "ISkyComponent.h"

#include <prev/core/device/Device.h>

namespace prev_test::component::sky {
class SkyComponentFactory final {
public:
    SkyComponentFactory(prev::core::device::Device& device, bool async = true);

    ~SkyComponentFactory() = default;

public:
    std::unique_ptr<ISkyComponent> Create() const;

private:
    prev::core::device::Device& m_device;

    bool m_async{ true };
};
} // namespace prev_test::component::sky

#endif // !__SKY_COMPONENT_FACTORY_H__
