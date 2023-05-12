#include "ParticleSystemComponent.h"

#include <random>

namespace prev_test::component::particle {
ParticleSystemComponent::ParticleSystemComponent(const std::shared_ptr<prev_test::render::IModel>& model, const std::shared_ptr<prev_test::render::IMaterial>& material, const std::shared_ptr<IParticleFactory>& particleFactory, const float particlesPerSecond)
    : m_model(model)
    , m_material(material)
    , m_particleFactory(particleFactory)
    , m_particlesPerSecond(particlesPerSecond)
{
}

void ParticleSystemComponent::Update(const float deltaTime, const glm::vec3& centerPosition)
{
    AddNewParticles(deltaTime, centerPosition);
    UpdateParticles(deltaTime);
}

void ParticleSystemComponent::SetParticlesPerSecond(const float pps)
{
    m_particlesPerSecond = pps;
}

float ParticleSystemComponent::GetParticlesPerSecond() const
{
    return m_particlesPerSecond;
}

std::shared_ptr<IParticleFactory> ParticleSystemComponent::GetParticleFactory() const
{
    return m_particleFactory;
}

std::shared_ptr<prev_test::render::IModel> ParticleSystemComponent::GetModel() const
{
    return m_model;
}

std::shared_ptr<prev_test::render::IMaterial> ParticleSystemComponent::GetMaterial() const
{
    return m_material;
}

std::list<std::shared_ptr<Particle>> ParticleSystemComponent::GetParticles() const
{
    return m_particles;
}

void ParticleSystemComponent::AddNewParticles(const float deltaTime, const glm::vec3& centerPosition)
{
    const float particlesToCreate = m_particlesPerSecond * deltaTime;
    const auto particlesToCreateCount = static_cast<int>(floorf(particlesToCreate));
    for (auto i = 0; i < particlesToCreateCount; i++) {
        m_particles.emplace_back(m_particleFactory->EmitParticle(centerPosition));
    }

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(0.0, 1.0);

    float partialCount = fmodf(particlesToCreate, 1.0f);
    if (dist(mt) < partialCount) {
        m_particles.emplace_back(m_particleFactory->EmitParticle(centerPosition));
    }
}

void ParticleSystemComponent::UpdateParticles(const float deltaTime)
{
    for (auto pi = m_particles.begin(); pi != m_particles.end();) {
        auto& particle = (*pi);
        particle->Update(deltaTime);
        if (!particle->IsAlive()) {
            pi = m_particles.erase(pi);
        } else {
            pi++;
        }
    }
}
} // namespace prev_test::component::particle