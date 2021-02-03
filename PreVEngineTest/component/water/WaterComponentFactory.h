#ifndef __WATER_COMPONENT_FACTORY_H__
#define __WATER_COMPONENT_FACTORY_H__

#include "IWaterComponent.h"
#include "IWaterOffscreenRenderPassComponent.h"

#include <map>

namespace prev_test::component::water {
class WaterComponentFactory final {
public:
    std::unique_ptr<IWaterComponent> Create(const int x, const int z) const;

    std::unique_ptr<IWaterOffscreenRenderPassComponent> CreateOffScreenComponent(const uint32_t w, const uint32_t h) const;

private:
    std::unique_ptr<prev_test::render::IModel> CreateModel(prev::core::memory::Allocator& allocator) const;
};
} // namespace prev_test::component::water

#endif
