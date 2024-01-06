#ifndef __SKY_COMPONENT_H__
#define __SKY_COMPONENT_H__

#include "ISkyComponent.h"

namespace prev_test::component::sky {
class SkyComponentFactory;

class SkyComponent : public ISkyComponent {
public:
    std::shared_ptr<prev_test::render::IModel> GetModel() const override;

    std::shared_ptr<prev::render::buffer::image::IImageBuffer> GetWeather() const override;

    std::shared_ptr<prev::render::sampler::Sampler> GetWeatherSampler() const override;

    std::shared_ptr<prev::render::buffer::image::IImageBuffer> GetPerlinWorleyNoise() const override;

    std::shared_ptr<prev::render::sampler::Sampler> GetPerlinWorleyNoiseSampler() const override;

    const glm::vec3& GetBottomColor() const override;

    const glm::vec3& GetTopColor() const override;

    const glm::vec3& GetCloudBaseColor() const override;

private:
    friend class SkyComponentFactory;

private:
    std::shared_ptr<prev_test::render::IModel> m_model{};

    std::shared_ptr<prev::render::buffer::image::IImageBuffer> m_weatherImageBuffer{};

    std::shared_ptr<prev::render::sampler::Sampler> m_weatehrImageSampler{};

    std::shared_ptr<prev::render::buffer::image::IImageBuffer> m_perlinWorleyNoiseImageBuffer{};

    std::shared_ptr<prev::render::sampler::Sampler> m_perlinWorleyNoiseSampler{};

    glm::vec3 m_bottomColor{};

    glm::vec3 m_topColor{};

    glm::vec3 m_cloudBaseColor{};
};
} // namespace prev_test::component::sky

#endif // !__SKY_COMPONENT_H__