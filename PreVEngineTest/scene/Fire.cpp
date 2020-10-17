#include "Fire.h"
#include "../component/particle/ParticleSystemComponentFactory.h"
#include "../component/terrain/ITerrainManagerComponent.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene {
void Fire::Init()
{
    prev_test::component::particle::ParticleSystemComponentFactory particleSystemComponentFactory{};
    m_particleSystemComponent = particleSystemComponentFactory.CreateRandomInCone(glm::vec3(0.0f, 1.0f, 0.0f), 15.0f);
    prev::scene::component::NodeComponentHelper::AddComponent<SceneNodeFlags, prev_test::component::particle::IParticleSystemComponent>(GetThis(), m_particleSystemComponent, SceneNodeFlags::PARTICLE_SYSTEM_COMPONENT);

    SceneNode::Init();
}

void Fire::Update(float deltaTime)
{
    const auto terrain = prev::scene::component::NodeComponentHelper::FindOne<SceneNodeFlags, prev_test::component::terrain::ITerrainManagerComponent>(prev::common::FlagSet<SceneNodeFlags>{ SceneNodeFlags::TERRAIN_MANAGER_COMPONENT });

    float height = 0.0f;
    terrain->GetHeightAt(m_initialPosition, height);

    m_particleSystemComponent->Update(deltaTime, glm::vec3(m_initialPosition.x, height - 4.0f, m_initialPosition.z));

    SceneNode::Update(deltaTime);
}

void Fire::ShutDown()
{
    SceneNode::ShutDown();
}
} // namespace prev_test::scene