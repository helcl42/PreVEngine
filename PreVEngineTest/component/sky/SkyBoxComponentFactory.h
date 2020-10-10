#ifndef __SKY_BOX_COMPONENT_FACTORY_H__
#define __SKY_BOX_COMPONENT_FACTORY_H__

#include "ISkyBoxComponent.h"

namespace prev_test::component::sky {
class SkyBoxComponentFactory final {
public:
    std::unique_ptr<ISkyBoxComponent> Create() const;

private:
    std::unique_ptr<prev_test::render::IModel> CreateModel(prev::core::memory::Allocator& allocator) const;

    std::unique_ptr<prev_test::render::IMaterial> CreateMaterial(prev::core::memory::Allocator& allocator, const std::vector<std::string>& textureFilenames) const;
};
} // namespace prev_test::component::sky

#endif // !__SKY_BOX_COMPONENT_FACTORY_H__
