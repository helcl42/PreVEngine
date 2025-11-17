#include "RandomInConeParticleFactory.h"

namespace prev_test::component::particle {
RandomInConeParticleFactory::RandomInConeParticleFactory(const std::shared_ptr<prev_test::render::IMaterial>& mt, const float gravityComp, const float avgSpeed, const float avgLifeLength, const float avgScale)
    : AbstractParticleFactory(mt, gravityComp, avgSpeed, avgLifeLength, avgScale)
{
}

void RandomInConeParticleFactory::SetConeDirection(const glm::vec3& direction)
{
    m_coneDirection = direction;
}

const glm::vec3& RandomInConeParticleFactory::GetConeDirection() const
{
    return m_coneDirection;
}

void RandomInConeParticleFactory::SetConeDirectionDeviation(const float deviationInDegs)
{
    m_directionDeviationInDegs = deviationInDegs;
}

float RandomInConeParticleFactory::GetConeDirectionDeviation() const
{
    return m_directionDeviationInDegs;
}

glm::vec3 RandomInConeParticleFactory::GenerateVelocty() const
{
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    float cosAngle = cosf(glm::radians(m_directionDeviationInDegs));
    float theta = dist(m_rng.GetRandomEngine()) * 2.0f * glm::pi<float>();
    float z = cosAngle + (dist(m_rng.GetRandomEngine()) * (1.0f - cosAngle));
    float rootOneMinusZSquared = sqrtf(1.0f - z * z);
    float x = rootOneMinusZSquared * cosf(theta);
    float y = rootOneMinusZSquared * sinf(theta);

    glm::vec4 direction(x, y, z, 1.0f);
    if (m_coneDirection.x != 0.0f || m_coneDirection.y != 0.0f || (m_coneDirection.z != 1.0f && m_coneDirection.z != -1.0f)) {
        const auto rotateAxis = glm::normalize(glm::cross(m_coneDirection, glm::vec3(0, 0, 1)));
        const auto rotateAngle = acosf(glm::dot(m_coneDirection, glm::vec3(0, 0, 1)));
        glm::mat4 rotationMatrix(1.0f);
        rotationMatrix = glm::rotate(rotationMatrix, -rotateAngle, rotateAxis);
        direction = rotationMatrix * direction;
    } else if (m_coneDirection.z == -1.0f) {
        direction.z *= -1.0;
    }
    return direction;
}

glm::vec3 RandomInConeParticleFactory::GenerateRadiusOffset() const
{
    glm::vec3 normalToCone;
    if (m_coneDirection.z != 1.0f && m_coneDirection.z != -1.0f) {
        normalToCone = glm::normalize(glm::cross(m_coneDirection, glm::vec3(0.0f, 0.0f, 1.0f)));
    } else {
        normalToCone = glm::normalize(glm::cross(m_coneDirection, glm::vec3(1.0f, 0.0f, 0.0f)));
    }

    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    const float theta = dist(m_rng.GetRandomEngine()) * 2.0f * glm::pi<float>();
    glm::mat4 rotationMat = glm::rotate(glm::mat4(1.0), theta, m_coneDirection);
    glm::vec3 offset = glm::normalize(rotationMat * glm::vec4(normalToCone, 1.0f)) * dist(m_rng.GetRandomEngine()) * m_radius;
    return offset;
}
} // namespace prev_test::component::particle