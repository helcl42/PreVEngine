#include "SkyComponent.h"

namespace prev_test::component::sky {
std::shared_ptr<prev_test::render::IModel> SkyComponent::GetModel() const
{
    return m_model;
}

std::shared_ptr<prev::render::buffer::ImageBuffer> SkyComponent::GetWeather() const
{
    return m_weatherImageBuffer;
}

std::shared_ptr<prev::render::buffer::ImageBuffer> SkyComponent::GetPerlinWorleyNoise() const
{
    return m_perlinWorleyNoiseImageBuffer;
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

float SkyComponent::GetElapsedTime() const
{
    return m_elapsedTime;
}

void SkyComponent::Update(float deltaTime)
{
    m_elapsedTime += deltaTime;
}
} // namespace prev_test::component::sky