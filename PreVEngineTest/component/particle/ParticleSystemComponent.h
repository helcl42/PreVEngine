#ifndef __PARTICLE_SYSTEM_COMPONENT_H__
#define __PARTICLE_SYSTEM_COMPONENT_H__

#include "IParticleSystemComponent.h"

namespace prev_test::component::particle {
class ParticleSystemComponent : public IParticleSystemComponent {
public:
    ParticleSystemComponent(const std::shared_ptr<prev_test::render::IModel>& model, const std::shared_ptr<prev_test::render::IMaterial>& material, const std::shared_ptr<IParticleFactory>& particleFactory, const float particlesPerSecond);

    virtual ~ParticleSystemComponent() = default;

public:
    void Update(const float deltaTime, const glm::vec3& centerPosition) override;

    void SetParticlesPerSecond(const float pps) override;

    float GetParticlesPerSecond() const override;

    std::shared_ptr<IParticleFactory> GetParticleFactory() const override;

    std::shared_ptr<prev_test::render::IModel> GetModel() const override;

    std::shared_ptr<prev_test::render::IMaterial> GetMaterial() const override;

    std::list<std::shared_ptr<Particle> > GetParticles() const override;

private:
    void AddNewParticles(const float deltaTime, const glm::vec3& centerPosition);

    void UpdateParticles(const float deltaTime);

private:
    const std::shared_ptr<prev_test::render::IModel> m_model;

    const std::shared_ptr<prev_test::render::IMaterial> m_material;

    const std::shared_ptr<IParticleFactory> m_particleFactory;

    std::list<std::shared_ptr<Particle> > m_particles;

    float m_particlesPerSecond;
};
} // namespace prev_test::component::particle

#endif // !__PARTICLE_SYSTEM_COMPONENT_H__
