#include "ParticleSystemComponentFactory.h"
#include "ParticleSystemComponent.h"
#include "RandomDirectionParticleFactory.h"
#include "RandomInConeParticleFactory.h"

#include "../../common/AssetManager.h"
#include "../../render/material/MaterialFactory.h"
#include "../../render/mesh/MeshFactory.h"
#include "../../render/model/ModelFactory.h"

#include <prev/scene/AllocatorProvider.h>

namespace prev_test::component::particle {
std::unique_ptr<IParticleSystemComponent> ParticleSystemComponentFactory::CreateRandom() const
{
    auto allocator{ prev::scene::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};

    std::shared_ptr<prev_test::render::IMaterial> material{ materialFactory.Create({ glm::vec4{ 1.0f }, 0.0f, 0.0f, true }, prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fire-ember-particles-png-4-transparent.png"), *allocator) };
    material->SetAtlasNumberOfRows(8);

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh{ meshFactory.CreateQuad() };

    prev_test::render::model::ModelFactory modelFactory{};
    auto model{ modelFactory.Create(std::move(mesh), *allocator) };

    auto particleFactory{ std::make_shared<RandomDirectionParticleFactory>(material, 0.1f, 5.0f, 4.0f, 10.0f) };
    particleFactory->SetRandomRotationEnabled(true);
    particleFactory->SetLifeLengthError(0.1f);
    particleFactory->SetSpeedError(0.25f);
    particleFactory->SetScaleError(0.1f);

    return std::make_unique<ParticleSystemComponent>(std::move(model), material, particleFactory, 10.0f);
}

std::unique_ptr<IParticleSystemComponent> ParticleSystemComponentFactory::CreateRandomInCone(const glm::vec3& coneDirection, const float angle) const
{
    auto allocator{ prev::scene::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::material::MaterialFactory materialFactory{};
    std::shared_ptr<prev_test::render::IMaterial> material{ materialFactory.Create({ glm::vec4{ 1.0f }, 0.0f, 0.0f, true }, prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fire-texture-atlas.png"), *allocator) };
    material->SetAtlasNumberOfRows(4);

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh{ meshFactory.CreateQuad() };

    prev_test::render::model::ModelFactory modelFactory{};
    auto model{ modelFactory.Create(std::move(mesh), *allocator) };

    auto particleFactory{ std::make_shared<RandomInConeParticleFactory>(material, -0.1f, 4.0f, 4.0f, 7.0f) };
    particleFactory->SetConeDirection(glm::vec3(0.0f, 1.0f, 0.0f));
    particleFactory->SetConeDirectionDeviation(1.0f);
    //particleFactory->SetRandomRotationEnabled(true);
    particleFactory->SetLifeLengthError(0.5f);
    particleFactory->SetSpeedError(1.0f);
    particleFactory->SetScaleError(2.0f);
    particleFactory->SetRadius(10.0f);

    return std::make_unique<ParticleSystemComponent>(std::move(model), material, particleFactory, 120.0f);
}
} // namespace prev_test::component::particle