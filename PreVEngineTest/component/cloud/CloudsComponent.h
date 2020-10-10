#ifndef __CLOUDS_COMPONENT_H__
#define __CLOUDS_COMPONENT_H__

#include "ICloudsComponent.h"

namespace prev_test::component::cloud {
class CloudsComponentFactory;

class CloudsComponent : public ICloudsComponent {
public:
    std::shared_ptr<prev::core::memory::image::IImageBuffer> GetPerlineNoise() const override;

    std::shared_ptr<prev::core::memory::image::IImageBuffer> GetWeather() const override;

    const glm::vec4& GetColor() const override;

private:
    friend class CloudsComponentFactory;

private:
    std::shared_ptr<prev::core::memory::image::IImageBuffer> m_weatherImageBuffer;

    std::shared_ptr<prev::core::memory::image::IImageBuffer> m_perlinWorleyNoiseImageBuffer;

    glm::vec4 m_color;
};
} // namespace prev_test::component::cloud

#endif // !__CLOUDS_COMPONENT_H__
