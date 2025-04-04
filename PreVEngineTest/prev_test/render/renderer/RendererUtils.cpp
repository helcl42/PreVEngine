#include "RendererUtils.h"

#include "../../component/ray_casting/IBoundingVolumeComponent.h"
#include "../../component/ray_casting/ISelectableComponent.h"
#include "../../component/ray_casting/RayCastingCommon.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer {
bool IsVisible(const prev_test::common::intersection::Frustum* frustums, const uint32_t frustumCount, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    bool visible{ true };
    if (auto boundingVolumeComponent = prev::scene::component::NodeComponentHelper::FindComponent<prev_test::component::ray_casting::IBoundingVolumeComponent>(node)) {
        bool checkedVisible{ false };
        for (uint32_t view = 0; view < frustumCount; ++view) {
            const auto& frustum{ frustums[view] };
            checkedVisible |= boundingVolumeComponent->IsInFrustum(frustum);
        }
        visible = checkedVisible;
    }
    return visible;
}

bool IsSelected(const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    bool selected{ false };
    if (auto selectableComponent = prev::scene::component::NodeComponentHelper::FindComponent<prev_test::component::ray_casting::ISelectableComponent>(node)) {
        selected = selectableComponent->IsSelected();
    }
    return selected;
}
} // namespace prev_test::render::renderer