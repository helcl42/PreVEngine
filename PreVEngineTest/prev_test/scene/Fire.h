#ifndef __FIRE_H__
#define __FIRE_H__

#include "../General.h"
#include "../component/particle/IParticleSystemComponent.h"
#include "../component/ray_casting/IBoundingVolumeComponent.h"

#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene {
class Fire final : public prev::scene::graph::SceneNode {
public:
    Fire(const glm::vec3& initPosition);

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    const glm::vec3 m_initialPosition;

    std::shared_ptr<prev_test::component::particle::IParticleSystemComponent> m_particleSystemComponent;

    std::shared_ptr<prev_test::component::ray_casting::IBoundingVolumeComponent> m_boundingVolumeComponent;
};
} // namespace prev_test::scene

#endif // !__FIRE_H__
