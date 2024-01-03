#include "ParticleSystemComponent.h"

#include "../../render/VertexDataBuffer.h"

#include <random>

namespace prev_test::component::particle {
namespace {
    uint32_t GetStride()
    {
        // position + scale + rotation + currentTextureOffset + nextTextureOffset + blendFactor
        return sizeof(glm::vec3) + sizeof(glm::vec2) + sizeof(float) + sizeof(glm::vec2) + sizeof(glm::vec2) + sizeof(float);
    }
} // namespace

ParticleSystemComponent::ParticleSystemComponent(const std::shared_ptr<prev_test::render::IModel>& model, const std::vector<std::shared_ptr<prev::render::buffer::VertexBuffer>>& vertexBuffers, const std::shared_ptr<prev_test::render::IMaterial>& material, const std::shared_ptr<IParticleFactory>& particleFactory, const float particlesPerSecond)
    : m_model(model)
    , m_vertexBuffers(vertexBuffers)
    , m_material(material)
    , m_particleFactory(particleFactory)
    , m_particlesPerSecond(particlesPerSecond)
    , m_currentBufferIndex(0)
{
}

void ParticleSystemComponent::Update(const float deltaTime, const glm::vec3& centerPosition)
{
    AddNewParticles(deltaTime, centerPosition);
    UpdateParticles(deltaTime);

    m_currentBufferIndex = (m_currentBufferIndex + 1) % static_cast<uint32_t>(m_vertexBuffers.size());

    prev_test::render::VertexDataBuffer instanceDataBuffer(GetStride() * m_particles.size());
    for (const auto& particle : m_particles) {
        instanceDataBuffer.Add(particle->GetPosition());
        instanceDataBuffer.Add(glm::vec2(particle->GetScale())); // TODO do we want non-square particles ??
        instanceDataBuffer.Add(particle->GetRotation());
        instanceDataBuffer.Add(particle->GetCurrentStageTextureOffset());
        instanceDataBuffer.Add(particle->GetNextStageTextureOffset());
        instanceDataBuffer.Add(particle->GetStagesBlendFactor());
    }

    auto& currentVetexBuffer{ m_vertexBuffers[m_currentBufferIndex] };
    currentVetexBuffer->Data(instanceDataBuffer.GetData(), static_cast<uint32_t>(m_particles.size()), GetStride());
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

std::shared_ptr<prev::render::buffer::VertexBuffer> ParticleSystemComponent::GetVertexBuffer() const
{
    return m_vertexBuffers[m_currentBufferIndex];
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