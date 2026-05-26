#ifndef __STONE_H__
#define __STONE_H__

#include "../component/ray_casting/IBoundingVolumeComponent.h"
#include "../component/transform/ITransformComponent.h"

#include <prev/core/device/Device.h>
#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene {
class Stone final : public prev::scene::graph::SceneNode {
public:
    Stone(prev::core::device::Device& device, const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale);

    ~Stone() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    prev::core::device::Device& m_device;

    glm::vec3 m_initialPosition;

    glm::quat m_initialOrientation;

    glm::vec3 m_initialScale;

    std::shared_ptr<prev_test::component::transform::ITransformComponent> m_transformComponent;

    std::shared_ptr<prev_test::component::ray_casting::IBoundingVolumeComponent> m_boundingVolumeComponent;
};
} // namespace prev_test::scene

#endif // !__STONE_H__
