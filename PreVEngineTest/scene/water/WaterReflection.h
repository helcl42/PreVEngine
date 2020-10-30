#ifndef __WATER_REFLECTION_H__
#define __WATER_REFLECTION_H__

#include "../../General.h"
#include "../../component/water/IWaterOffscreenRenderPassComponent.h"

#include <prev/core/CoreEvents.h>
#include <prev/event/EventHandler.h>
#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene::water {
class WaterReflection final : public prev::scene::graph::SceneNode {
public:
    WaterReflection();

    ~WaterReflection() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

public:
    void operator()(const prev::core::NewIterationEvent& newIterationEvent);

private:
    void CreateReflectionComponent();

    void DestroyReflectionComponent();

private:
    std::shared_ptr<prev_test::component::water::IWaterOffscreenRenderPassComponent> m_reflectionComponent;

    glm::uvec2 m_viewPortSize{ 0, 0 };

    glm::uvec2 m_previousViewPortSize{ 1920, 1080 };

private:
    prev::event::EventHandler<WaterReflection, prev::core::NewIterationEvent> m_newIterationHandler{ *this };
};
} // namespace prev_test::scene::water

#endif // !__WATER_REFLECTION_H__