#ifndef __LENS_FLARE_COMPONENT_FACTORY_H__
#define __LENS_FLARE_COMPONENT_FACTORY_H__

#include "ILensFlareComponent.h"

#include <prev/core/device/Device.h>

namespace prev_test::component::sky {
class LensFlareComponentFactory final {
public:
    // async: when true (default), the model and flare textures stream in asynchronously.
    LensFlareComponentFactory(prev::core::device::Device& device, bool async = true);

public:
    std::unique_ptr<ILensFlareComponent> Create() const;

private:
    prev::core::device::Device& m_device;

    bool m_async{ true };
};
} // namespace prev_test::component::sky

#endif // !__LENS_FLARE_COMPONENT_FACTORY_H__
