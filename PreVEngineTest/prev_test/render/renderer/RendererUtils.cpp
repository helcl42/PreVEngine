#include "RendererUtils.h"

#include "../../component/ray_casting/IBoundingVolumeComponent.h"
#include "../../component/ray_casting/ISelectableComponent.h"
#include "../../component/ray_casting/RayCastingCommon.h"

#include <prev/scene/component/ComponentRepository.h>

namespace prev_test::render::renderer {
bool IsVisible(const prev_test::common::intersection::Frustum* frustums, const uint32_t frustumCount, const uint64_t nodeId)
{
    bool visible{ true };
    if (auto boundingVolumeComponent = prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IBoundingVolumeComponent>::Instance().FindFirst(nodeId)) {
        bool checkedVisible{ false };
        for (uint32_t view = 0; view < frustumCount; ++view) {
            const auto& frustum{ frustums[view] };
            checkedVisible |= boundingVolumeComponent->IsInFrustum(frustum);
        }
        visible = checkedVisible;
    }
    return visible;
}

bool IsSelected(const uint64_t nodeId)
{
    bool selected{ false };
    if (auto selectableComponent = prev::scene::component::ComponentRepository<prev_test::component::ray_casting::ISelectableComponent>::Instance().FindFirst(nodeId)) {
        selected = selectableComponent->IsSelected();
    }
    return selected;
}
} // namespace prev_test::render::renderer