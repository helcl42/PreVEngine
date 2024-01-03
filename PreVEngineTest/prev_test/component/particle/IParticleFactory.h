#ifndef __IPARTICLE_FACTORY_H__
#define __IPARTICLE_FACTORY_H__

#include "Particle.h"

namespace prev_test::component::particle {
class IParticleFactory {
public:
    virtual std::unique_ptr<Particle> EmitParticle(const glm::vec3& centerPosition) const = 0;

public:
    virtual ~IParticleFactory() = default;
};
} // namespace prev_test::component::particle

#endif // !__IPARTICLE_FACTORY_H__
