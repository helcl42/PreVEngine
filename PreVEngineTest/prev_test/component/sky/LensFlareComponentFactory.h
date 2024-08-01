#ifndef __LENS_FLARE_COMPONENT_FACTORY_H__
#define __LENS_FLARE_COMPONENT_FACTORY_H__

#include "ILensFlareComponent.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>

namespace prev_test::component::sky {
class LensFlareComponentFactory final {
public:
    LensFlareComponentFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator);

public:
    std::unique_ptr<ILensFlareComponent> Create() const;

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

private:
    struct FlareCreateInfo {
        std::string path{};
        float scale{};
    };

    std::unique_ptr<Flare> CreateFlare(const std::string& filePath, const float scale) const;
};
} // namespace prev_test::component::sky

#endif // !__LENS_FLARE_COMPONENT_FACTORY_H__
