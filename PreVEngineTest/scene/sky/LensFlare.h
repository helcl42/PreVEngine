#ifndef __LENS_FLARE_H__
#define __LENS_FLARE_H__

#include "../../General.h"
#include "../../component/sky/ILensFlareComponent.h"

#include <prev/core/CoreEvents.h>
#include <prev/event/EventHandler.h>
#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene::sky {
class LensFlare final : public prev::scene::graph::SceneNode<SceneNodeFlags> {
public:
    LensFlare();

    ~LensFlare() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

public:
    void operator()(const prev::core::NewIterationEvent& newIterationEvent);

private:
    std::shared_ptr<prev_test::component::sky::ILensFlareComponent> m_lensFlareComponent;

    glm::vec2 m_viewPortSize;

private:
    prev::event::EventHandler<LensFlare, prev::core::NewIterationEvent> m_newIterationHandler{ *this };
};
} // namespace prev_test::scene::sky

#endif // !__LENS_FLARE_H__