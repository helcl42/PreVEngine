#include "AbstractParticleFactory.h"

#include <random>

namespace prev_test::component::particle {
AbstractParticleFactory::AbstractParticleFactory(const std::shared_ptr<prev_test::render::IMaterial>& mt, const float gravityComp, const float avgSpeed, const float avgLifeLength, const float avgScale)
    : m_material(mt)
    , m_gravityCompliment(gravityComp)
    , m_averageSpeed(avgSpeed)
    , m_averageLifeLength(avgLifeLength)
    , m_averageScale(avgScale)
{
}

std::unique_ptr<Particle> AbstractParticleFactory::EmitParticle(const glm::vec3& centerPosition) const
{
    glm::vec3 velocity = GenerateVelocty();
    velocity = glm::normalize(velocity);
    velocity *= GenerateValue(m_averageSpeed, m_speedError);
    float lifeLength = GenerateValue(m_averageLifeLength, m_lifeLengthError);
    float rotation = m_randomRotation ? GenerateRotation() : 0.0f;
    float scale = GenerateValue(m_averageScale, m_scaleError);
    glm::vec3 radiusOffset = GenerateRadiusOffset();
    return std::make_unique<Particle>(m_material, centerPosition + radiusOffset, velocity, m_gravityCompliment, lifeLength, rotation, scale);
}

float AbstractParticleFactory::GenerateRotation()
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(0.0f, 360.0f);
    return dist(mt);
}

float AbstractParticleFactory::GenerateValue(const float average, const float errorMargin)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    float offset = dist(mt) * errorMargin;
    return average + offset;
}

void AbstractParticleFactory::SetSpeedError(const float err)
{
    m_speedError = err;
}

float AbstractParticleFactory::GetSpeedError() const
{
    return m_speedError;
}

void AbstractParticleFactory::SetLifeLengthError(const float err)
{
    m_lifeLengthError = err;
}

float AbstractParticleFactory::GetLifeLengthError() const
{
    return m_lifeLengthError;
}

void AbstractParticleFactory::SetScaleError(const float err)
{
    m_scaleError = err;
}

float AbstractParticleFactory::GetScaleError() const
{
    return m_scaleError;
}

void AbstractParticleFactory::SetRandomRotationEnabled(bool enabled)
{
    m_randomRotation = enabled;
}

bool AbstractParticleFactory::IsRandomRotationEnabled() const
{
    return m_randomRotation;
}

void AbstractParticleFactory::SetRadius(const float radius)
{
    m_radius = radius;
}

float AbstractParticleFactory::GetRadius() const
{
    return m_radius;
}
} // namespace prev_test::component::particle
