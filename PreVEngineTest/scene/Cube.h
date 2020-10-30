#ifndef __CUBE_H__
#define __CUBE_H__

#include "../General.h"
#include "../component/ray_casting/IBoundingVolumeComponent.h"
#include "../component/transform/ITransformComponent.h"

#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene {
class Cube final : public prev::scene::graph::SceneNode {
public:
    Cube(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::string& texturePath, const std::string& normalMapPath, const std::string& heightMapPath, const float heightScale);

    virtual ~Cube() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

protected:
    const glm::vec3 m_initialPosition;

    const glm::quat m_initialOrientation;

    const glm::vec3 m_initialScale;

    const std::string m_texturePath;

    const std::string m_normalMapPath;

    const std::string m_heightMapPath;

    const float m_heightScale;

    std::shared_ptr<prev_test::component::transform::ITransformComponent> m_transformComponent;

    std::shared_ptr<prev_test::component::ray_casting::IBoundingVolumeComponent> m_boundingVolumeComponent;
};
} // namespace prev_test::scene

#endif // !__CUBE_H__
