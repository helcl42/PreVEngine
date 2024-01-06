#include "SkyComponent.h"

namespace prev_test::component::sky {
std::shared_ptr<prev_test::render::IModel> SkyComponent::GetModel() const
{
    return m_model;
}

std::shared_ptr<prev::render::buffer::image::IImageBuffer> SkyComponent::GetWeather() const
{
    return m_weatherImageBuffer;
}

std::shared_ptr<prev::render::sampler::Sampler> SkyComponent::GetWeatherSampler() const
{
    return m_weatehrImageSampler;
}

std::shared_ptr<prev::render::buffer::image::IImageBuffer> SkyComponent::GetPerlinWorleyNoise() const
{
    return m_perlinWorleyNoiseImageBuffer;
}

std::shared_ptr<prev::render::sampler::Sampler> SkyComponent::GetPerlinWorleyNoiseSampler() const
{
    return m_perlinWorleyNoiseSampler;
}

const glm::vec3& SkyComponent::GetBottomColor() const
{
    return m_bottomColor;
}

const glm::vec3& SkyComponent::GetTopColor() const
{
    return m_topColor;
}

const glm::vec3& SkyComponent::GetCloudBaseColor() const
{
    return m_cloudBaseColor;
}
} // namespace prev_test::component::sky