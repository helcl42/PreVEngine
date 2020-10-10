#ifndef __RANDOM_IN_CONE_PARTICLE_FACTORY_H__
#define __RANDOM_IN_CONE_PARTICLE_FACTORY_H__

#include "AbstractParticleFactory.h"

namespace prev_test::component::particle {
class RandomInConeParticleFactory final : public AbstractParticleFactory {
public:
    RandomInConeParticleFactory(const std::shared_ptr<prev_test::render::IMaterial>& mt, const float gravityComp, const float avgSpeed, const float avgLifeLength, const float avgScale);

    ~RandomInConeParticleFactory() = default;

public:
    void SetConeDirection(const glm::vec3& direction);

    const glm::vec3& GetConeDirection() const;

    void SetConeDirectionDeviation(const float deviationInDegs);

    float GetConeDirectionDeviation() const;

protected:
    glm::vec3 GenerateVelocty() const override;

    glm::vec3 GenerateRadiusOffset() const override;

private:
    glm::vec3 m_coneDirection{ 0.0f, 1.0f, 0.0f };

    float m_directionDeviationInDegs{ 20.0f };
};
} // namespace prev_test::component::particle

#endif // !__RANDOM_IN_CONE_PARTICLE_FACTORY_H__
