#ifndef __CUBE_ROBOT_BASE_H__
#define __CUBE_ROBOT_BASE_H__

#include "../../General.h"
#include "../../component/ray_casting/IBoundingVolumeComponent.h"
#include "../../component/transform/ITransformComponent.h"

#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene::robot {
class CubeRobotPart : public prev::scene::graph::SceneNode {
public:
    CubeRobotPart(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::string& texturePath);

    virtual ~CubeRobotPart() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

protected:
    const std::string m_texturePath;

    const glm::vec3 m_initialPosition;

    const glm::quat m_initialOrientation;

    const glm::vec3 m_initialScale;

private:
    std::shared_ptr<prev_test::component::transform::ITransformComponent> m_transformComponent;

    std::shared_ptr<prev_test::component::ray_casting::IBoundingVolumeComponent> m_boundingVolumeComponent;
};

} // namespace prev_test::scene::robot

#endif // !__CUBE_ROBOT_BASE_H__
