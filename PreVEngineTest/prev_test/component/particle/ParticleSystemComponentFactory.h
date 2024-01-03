#ifndef __PARTICLE_SYSTEM_COMPONENT_FACTORY_H__
#define __PARTICLE_SYSTEM_COMPONENT_FACTORY_H__

#include "IParticleSystemComponent.h"

namespace prev_test::component::particle {
class ParticleSystemComponentFactory final {
public:
    std::unique_ptr<IParticleSystemComponent> CreateRandom() const;

    std::unique_ptr<IParticleSystemComponent> CreateRandomInCone(const glm::vec3& coneDirection, const float angle) const;
};
} // namespace prev_test::component::particle

#endif // !__PARTICLE_SYSTEM_COMPONENT_FACTORY_H__
