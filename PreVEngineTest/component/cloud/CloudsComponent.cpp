#include "CloudsComponent.h"

namespace prev_test::component::cloud {
std::shared_ptr<prev::core::memory::image::IImageBuffer> CloudsComponent::GetPerlineNoise() const
{
    return m_perlinWorleyNoiseImageBuffer;
}

std::shared_ptr<prev::core::memory::image::IImageBuffer> CloudsComponent::GetWeather() const
{
    return m_weatherImageBuffer;
}

const glm::vec4& CloudsComponent::GetColor() const
{
    return m_color;
}
} // namespace prev_test::component::cloud