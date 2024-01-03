#ifndef __RANDOM_DIRECTION_PARTICLE_FACTORY_H__
#define __RANDOM_DIRECTION_PARTICLE_FACTORY_H__

#include "AbstractParticleFactory.h"

namespace prev_test::component::particle {
class RandomDirectionParticleFactory final : public AbstractParticleFactory {
public:
    RandomDirectionParticleFactory(const std::shared_ptr<prev_test::render::IMaterial>& mt, const float gravityComp, const float avgSpeed, const float avgLifeLength, const float avgScale);

    ~RandomDirectionParticleFactory() = default;

protected:
    glm::vec3 GenerateVelocty() const override;

    glm::vec3 GenerateRadiusOffset() const override;
};
} // namespace prev_test::component::particle

#endif // !__RANDOM_DIRECTION_PARTICLE_FACTORY_H__
