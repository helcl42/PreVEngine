#ifndef __FIRE_H__
#define __FIRE_H__

#include "../General.h"
#include "../component/particle/IParticleSystemComponent.h"

#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene {
class Fire final : public prev::scene::graph::SceneNode<SceneNodeFlags> {
public:
    Fire(const glm::vec3& initPosition)
        : SceneNode()
        , m_initialPosition(initPosition)
    {
    }

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    const glm::vec3 m_initialPosition;

    std::shared_ptr<prev_test::component::particle::IParticleSystemComponent> m_particleSystemComponent;
};
} // namespace prev_test::scene

#endif // !__FIRE_H__
