#include "RandomDirectionParticleFactory.h"

namespace prev_test::component::particle {
RandomDirectionParticleFactory::RandomDirectionParticleFactory(const std::shared_ptr<prev_test::render::IMaterial>& mt, const float gravityComp, const float avgSpeed, const float avgLifeLength, const float avgScale)
    : AbstractParticleFactory(mt, gravityComp, avgSpeed, avgLifeLength, avgScale)
{
}

glm::vec3 RandomDirectionParticleFactory::GenerateVelocty() const
{
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    float theta = dist(m_rng.GetRandomEngine()) * 2.0f * glm::pi<float>();
    float z = dist(m_rng.GetRandomEngine()) * 2.0f - 1;
    float rootOneMinusZSquared = sqrtf(1.0f - z * z);
    float x = rootOneMinusZSquared * cosf(theta);
    float y = rootOneMinusZSquared * sinf(theta);
    return glm::vec3(x, y, z);
}

glm::vec3 RandomDirectionParticleFactory::GenerateRadiusOffset() const
{
    return glm::vec3{ GenerateValue(m_radius, 1.0f), GenerateValue(m_radius, 1.0f), GenerateValue(m_radius, 1.0f) };
}
} // namespace prev_test::component::particle