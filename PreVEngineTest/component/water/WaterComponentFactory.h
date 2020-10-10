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
    std::shared_ptr<prev::render::image::Image> CreateImage(const std::string& textureFilename) const;

    std::unique_ptr<prev_test::render::IMaterial> CreateMaterial(prev::core::memory::Allocator& allocator, const glm::vec4& color, const std::string& textureFilename, const std::string& normalTextureFilename, const float shineDamper, const float reflectivity) const;

    std::unique_ptr<prev_test::render::IModel> CreateModel(prev::core::memory::Allocator& allocator) const;

private:
    static inline std::map<std::string, std::shared_ptr<prev::render::image::Image> > s_waterImageCache;
};
} // namespace prev_test::component::water

#endif
