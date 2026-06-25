#ifndef __SKY_BOX_COMPONENT_FACTORY_H__
#define __SKY_BOX_COMPONENT_FACTORY_H__

#include "ISkyBoxComponent.h"

#include <prev/core/device/Device.h>

namespace prev_test::component::sky {
class SkyBoxComponentFactory final {
public:
    SkyBoxComponentFactory(prev::core::device::Device& device, bool async = true);

    ~SkyBoxComponentFactory() = default;

public:
    std::unique_ptr<ISkyBoxComponent> Create() const;

private:
    prev::core::device::Device& m_device;

    bool m_async{ true };
};
} // namespace prev_test::component::sky

#endif // !__SKY_BOX_COMPONENT_FACTORY_H__
