#include "Fire.h"

#include "../component/particle/ParticleSystemComponentFactory.h"
#include "../component/ray_casting/BoundingVolumeComponentFactory.h"
#include "../component/terrain/ITerrainManagerComponent.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene {
void Fire::Init()
{
    prev_test::component::particle::ParticleSystemComponentFactory particleSystemComponentFactory{};
    m_particleSystemComponent = particleSystemComponentFactory.CreateRandomInCone(glm::vec3(0.0f, 1.0f, 0.0f), 25.0f);
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::particle::IParticleSystemComponent>(GetThis(), m_particleSystemComponent, TAG_PARTICLE_SYSTEM_COMPONENT);

    prev_test::component::ray_casting::BoundingVolumeComponentFactory bondingVolumeFactory{};
    m_boundingVolumeComponent = bondingVolumeFactory.CreateAABB(prev_test::common::intersection::AABB(glm::vec3(-1.0), glm::vec3(1.0)), glm::vec3(6.0f, 15.0f, 6.0f), {});
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::ray_casting::IBoundingVolumeComponent>(GetThis(), m_boundingVolumeComponent, TAG_BOUNDING_VOLUME_COMPONENT);

    SceneNode::Init();
}

void Fire::Update(float deltaTime)
{
    const auto terrain = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::terrain::ITerrainManagerComponent>({ TAG_TERRAIN_MANAGER_COMPONENT });

    float height{ 0.0f };
    terrain->GetHeightAt(m_initialPosition, height);

    const glm::vec3 position{ m_initialPosition.x, height - 4.0f, m_initialPosition.z };

    m_particleSystemComponent->Update(deltaTime, position);
    m_boundingVolumeComponent->Update(glm::translate(glm::mat4(1.0f), position + glm::vec3(0.0f, 15.0f, 0.0f)));

    SceneNode::Update(deltaTime);
}

void Fire::ShutDown()
{
    SceneNode::ShutDown();
}
} // namespace prev_test::scene