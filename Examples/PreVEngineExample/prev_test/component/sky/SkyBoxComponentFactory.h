#ifndef __SKY_BOX_COMPONENT_FACTORY_H__
#define __SKY_BOX_COMPONENT_FACTORY_H__

#include "ISkyBoxComponent.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>

namespace prev_test::component::sky {
class SkyBoxComponentFactory final {
public:
    SkyBoxComponentFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator);

    ~SkyBoxComponentFactory() = default;

public:
    std::unique_ptr<ISkyBoxComponent> Create() const;

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;
};
} // namespace prev_test::component::sky

#endif // !__SKY_BOX_COMPONENT_FACTORY_H__
