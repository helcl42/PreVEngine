#ifndef __FIRE_H__
#define __FIRE_H__

#include "../component/particle/IParticleSystemComponent.h"
#include "../component/ray_casting/IBoundingVolumeComponent.h"

#include <prev/core/device/Device.h>
#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene {
class Fire final : public prev::scene::graph::SceneNode {
public:
    Fire(prev::core::device::Device& device, const glm::vec3& initPosition);

    ~Fire() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    prev::core::device::Device& m_device;

    glm::vec3 m_initialPosition;

    std::shared_ptr<prev_test::component::particle::IParticleSystemComponent> m_particleSystemComponent;

    std::shared_ptr<prev_test::component::ray_casting::IBoundingVolumeComponent> m_boundingVolumeComponent;
};
} // namespace prev_test::scene

#endif // !__FIRE_H__
