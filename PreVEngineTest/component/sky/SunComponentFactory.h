#ifndef __SUN_COMPONENT_FACTORY_H__
#define __SUN_COMPONENT_FACTORY_H__

#include "ISunComponent.h"

namespace prev_test::component::sky {
class SunComponentFactory final {
public:
    std::unique_ptr<ISunComponent> Create() const;

private:
    std::unique_ptr<prev_test::render::IModel> CreateModel(prev::core::memory::Allocator& allocator) const;

    std::unique_ptr<Flare> CreateFlare(prev::core::memory::Allocator& allocator, const std::string& filePath, const float scale) const;
};
} // namespace prev_test::component::sky

#endif // !__SUN_COMPONENT_FACTORY_H__
