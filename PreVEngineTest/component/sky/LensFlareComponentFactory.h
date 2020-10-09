#ifndef __LENS_FLARE_COMPONENT_FACTORY_H__
#define __LENS_FLARE_COMPONENT_FACTORY_H__

#include "ILensFlareComponent.h"

namespace prev_test::component::sky {
class LensFlareComponentFactory final {
public:
    std::unique_ptr<ILensFlareComponent> Create() const;

private:
    struct FlareCreateInfo {
        std::string path;
        float scale;
    };

    std::unique_ptr<prev_test::render::IModel> CreateModel(prev::core::memory::Allocator& allocator) const;

    std::unique_ptr<Flare> CreateFlare(prev::core::memory::Allocator& allocator, const std::string& filePath, const float scale) const;
};
} // namespace prev_test::component::sky

#endif // !__LENS_FLARE_COMPONENT_FACTORY_H__
