#ifndef __LENS_FLARE_COMPONENT_FACTORY_H__
#define __LENS_FLARE_COMPONENT_FACTORY_H__

#include "ILensFlareComponent.h"

#include <prev/core/device/Device.h>

namespace prev_test::component::sky {
class LensFlareComponentFactory final {
public:
    LensFlareComponentFactory(prev::core::device::Device& device);

public:
    std::unique_ptr<ILensFlareComponent> Create() const;

private:
    prev::core::device::Device& m_device;

};
} // namespace prev_test::component::sky

#endif // !__LENS_FLARE_COMPONENT_FACTORY_H__
