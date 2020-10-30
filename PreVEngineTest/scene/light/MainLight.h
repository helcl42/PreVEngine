#ifndef __NAIN_LIGHT_H__
#define __NAIN_LIGHT_H__

#include "../../General.h"
#include "../../component/light/ILightComponent.h"
#include "../../component/transform/ITransformComponent.h"

#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene::light {
class MainLight final : public prev::scene::graph::SceneNode {
public:
    MainLight(const glm::vec3& pos);

    ~MainLight() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    std::shared_ptr<prev_test::component::transform::ITransformComponent> m_transformComponent;

    std::shared_ptr<prev_test::component::light::ILightComponent> m_lightComponent;

    glm::vec3 m_initialPosition;
};
} // namespace prev_test::scene::light

#endif