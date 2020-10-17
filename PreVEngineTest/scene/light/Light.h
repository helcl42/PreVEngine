#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "../../General.h"
#include "../../component/light/ILightComponent.h"
#include "../../component/transform/ITransformComponent.h"

#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene::light {
class Light final : public prev::scene::graph::SceneNode<SceneNodeFlags> {
public:
    Light(const glm::vec3& position, const glm::vec3& color);

    ~Light() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    const glm::vec3 m_initialPosition;

    const glm::vec3 m_color;

    std::shared_ptr<prev_test::component::transform::ITransformComponent> m_transformComponent;

    std::shared_ptr<prev_test::component::light::ILightComponent> m_lightComponent;
};
} // namespace prev_test::scene::light

#endif // !__LIGHT_H__
