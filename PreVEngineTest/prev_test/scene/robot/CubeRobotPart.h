#ifndef __CUBE_ROBOT_BASE_H__
#define __CUBE_ROBOT_BASE_H__

#include "../../General.h"
#include "../../component/ray_casting/IBoundingVolumeComponent.h"
#include "../../component/transform/ITransformComponent.h"

#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene::robot {
class CubeRobotPart : public prev::scene::graph::SceneNode {
public:
    CubeRobotPart(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::string& texturePath);

    virtual ~CubeRobotPart() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

protected:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

protected:
    glm::vec3 m_initialPosition;

    glm::quat m_initialOrientation;

    glm::vec3 m_initialScale;

    std::string m_texturePath;

private:
    std::shared_ptr<prev_test::component::transform::ITransformComponent> m_transformComponent;

    std::shared_ptr<prev_test::component::ray_casting::IBoundingVolumeComponent> m_boundingVolumeComponent;
};

} // namespace prev_test::scene::robot

#endif // !__CUBE_ROBOT_BASE_H__
