#ifndef __IPARTICLE_SYSTEM_COMPONENT_H__
#define __IPARTICLE_SYSTEM_COMPONENT_H__

#include "../../render/IModel.h"
#include "IParticleFactory.h"

#include <list>

namespace prev_test::component::particle {
class IParticleSystemComponent {
public:
    virtual void Update(const float deltaTime, const glm::vec3& centerPosition) = 0;

    virtual void SetParticlesPerSecond(const float pps) = 0;

    virtual float GetParticlesPerSecond() const = 0;

    virtual std::shared_ptr<IParticleFactory> GetParticleFactory() const = 0;

    virtual std::shared_ptr<prev_test::render::IModel> GetModel() const = 0;

    virtual std::shared_ptr<prev_test::render::IMaterial> GetMaterial() const = 0;

    virtual std::list<std::shared_ptr<Particle>> GetParticles() const = 0;

public:
    virtual ~IParticleSystemComponent() = default;
};
} // namespace prev_test::component::particle

#endif // !__IPARTICLE_SYSTEM_COMPONENT_H__
