#ifndef __IHAND_TRACKING_COMPONENT_H__
#define __IHAND_TRACKING_COMPONENT_H__

#ifdef ENABLE_XR

#include "../../render/IModel.h"

#include <prev/scene/component/IComponent.h>
#include <prev/xr/XrEvents.h>

namespace prev_test::component::hand_tracking {
class IHandTrackingComponent : public prev::scene::component::IComponent {
public:
    virtual const prev::xr::HandsEvent& GetHandsData() const = 0;

    virtual std::shared_ptr<prev_test::render::IModel> GetJointModel() const = 0;

public:
    virtual ~IHandTrackingComponent() = default;
};
} // namespace prev_test::component::hand_tracking

#endif

#endif // !__IHAND_TRACKING_COMPONENT_H__
