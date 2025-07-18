#include "ParticleSystemComponentFactory.h"
#include "ParticleSystemComponent.h"
#include "RandomDirectionParticleFactory.h"
#include "RandomInConeParticleFactory.h"

#include "../../common/AssetManager.h"
#include "../../render/material/MaterialFactory.h"
#include "../../render/mesh/MeshFactory.h"
#include "../../render/model/ModelFactory.h"

#include <prev/render/buffer/VertexBuffer.h>

namespace prev_test::component::particle {

static const inline uint32_t BufferCount{ 2 };

static const inline uint32_t MaxParticleCount{ 100000 };

ParticleSystemComponentFactory::ParticleSystemComponentFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator)
    : m_device{ device }
    , m_allocator{ allocator }
{
}

std::unique_ptr<IParticleSystemComponent> ParticleSystemComponentFactory::CreateRandom() const
{
    prev_test::render::material::MaterialFactory materialFactory{ m_device, m_allocator };

    std::shared_ptr<prev_test::render::IMaterial> material{ materialFactory.Create({ glm::vec4{ 1.0f }, 0.0f, 0.0f }, prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fire-ember-particles-png-4-transparent.png")) };
    material->SetAtlasNumberOfRows(8);

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh{ meshFactory.CreateQuad() };

    prev_test::render::model::ModelFactory modelFactory{ m_allocator };
    auto model{ modelFactory.Create(std::move(mesh)) };

    std::vector<std::shared_ptr<prev::render::buffer::VertexBuffer>> vertexBuffers(BufferCount);
    for (uint32_t i = 0; i < BufferCount; ++i) {
        vertexBuffers[i] = std::make_shared<prev::render::buffer::HostMappedVertexBuffer>(m_allocator, MaxParticleCount);
    }

    auto particleFactory{ std::make_shared<RandomDirectionParticleFactory>(material, 0.1f, 5.0f, 4.0f, 10.0f) };
    particleFactory->SetRandomRotationEnabled(true);
    particleFactory->SetLifeLengthError(0.1f);
    particleFactory->SetSpeedError(0.25f);
    particleFactory->SetScaleError(0.1f);

    return std::make_unique<ParticleSystemComponent>(std::move(model), vertexBuffers, material, particleFactory, 10.0f);
}

std::unique_ptr<IParticleSystemComponent> ParticleSystemComponentFactory::CreateRandomInCone(const glm::vec3& coneDirection, const float angle) const
{
    prev_test::render::material::MaterialFactory materialFactory{ m_device, m_allocator };
    std::shared_ptr<prev_test::render::IMaterial> material{ materialFactory.Create({ glm::vec4{ 1.0f }, 0.0f, 0.0f }, prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fire-texture-atlas.png")) };
    material->SetAtlasNumberOfRows(4);

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh{ meshFactory.CreateQuad() };

    prev_test::render::model::ModelFactory modelFactory{ m_allocator };
    auto model{ modelFactory.Create(std::move(mesh)) };

    std::vector<std::shared_ptr<prev::render::buffer::VertexBuffer>> vertexBuffers(BufferCount);
    for (uint32_t i = 0; i < BufferCount; ++i) {
        vertexBuffers[i] = std::make_shared<prev::render::buffer::HostMappedVertexBuffer>(m_allocator, MaxParticleCount);
    }

    auto particleFactory{ std::make_shared<RandomInConeParticleFactory>(material, -0.1f, 4.0f, 4.0f, 7.0f) };
    particleFactory->SetConeDirection(coneDirection);
    particleFactory->SetConeDirectionDeviation(angle);
    // particleFactory->SetRandomRotationEnabled(true);
    particleFactory->SetLifeLengthError(0.5f);
    particleFactory->SetSpeedError(1.0f);
    particleFactory->SetScaleError(2.0f);
    particleFactory->SetRadius(10.0f);

    return std::make_unique<ParticleSystemComponent>(std::move(model), vertexBuffers, material, particleFactory, 100.0f);
}
} // namespace prev_test::component::particle