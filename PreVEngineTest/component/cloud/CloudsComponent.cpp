#include "CloudsComponent.h"

namespace prev_test::component::cloud {
std::shared_ptr<prev::core::memory::image::IImageBuffer> CloudsComponent::GetPerlinWorleyNoise() const
{
    return m_perlinWorleyNoiseImageBuffer;
}

std::shared_ptr<prev::render::sampler::Sampler> CloudsComponent::GetPerlinWorleyNoiseSampler() const
{
    return m_perlinWorleyNoiseSampler;
}

std::shared_ptr<prev::core::memory::image::IImageBuffer> CloudsComponent::GetWeather() const
{
    return m_weatherImageBuffer;
}

std::shared_ptr<prev::render::sampler::Sampler> CloudsComponent::GetWeatherSampler() const
{
    return m_weatehrImageSampler;
}

const glm::vec4& CloudsComponent::GetColor() const
{
    return m_color;
}
} // namespace prev_test::component::cloud