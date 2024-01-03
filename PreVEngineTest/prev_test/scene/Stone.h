#ifndef __STONE_H__
#define __STONE_H__

#include "../General.h"
#include "../component/ray_casting/IBoundingVolumeComponent.h"
#include "../component/transform/ITransformComponent.h"

#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene {
class Stone final : public prev::scene::graph::SceneNode {
public:
    Stone(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale);

    ~Stone() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    const glm::vec3 m_initialPosition;

    const glm::quat m_initialOrientation;

    const glm::vec3 m_initialScale;

    std::shared_ptr<prev_test::component::transform::ITransformComponent> m_transformComponent;

    std::shared_ptr<prev_test::component::ray_casting::IBoundingVolumeComponent> m_boundingVolumeComponent;
};
} // namespace prev_test::scene

#endif // !__STONE_H__
