#ifndef __HAND_TRACKING_COMPONENT_H__
#define __HAND_TRACKING_COMPONENT_H__

#ifdef ENABLE_XR

#include "IHandTrackingComponent.h"

#include <prev/core/device/Device.h>
#include <prev/event/EventHandler.h>

namespace prev_test::component::hand_tracking {
class HandTrackingComponent final : public IHandTrackingComponent {
public:
    HandTrackingComponent(prev::core::device::Device& device);

    ~HandTrackingComponent() = default;

public:
    const prev::xr::HandsEvent& GetHandsData() const override;

    std::shared_ptr<prev_test::render::IModel> GetJointModel() const override;

public:
    void operator()(const prev::xr::HandsEvent& handsEvent);

private:
    prev::xr::HandsEvent m_handsData{};

    std::shared_ptr<prev_test::render::IModel> m_jointModel;

    prev::event::EventHandler<HandTrackingComponent, prev::xr::HandsEvent> m_handsEventHandler{ *this };
};
} // namespace prev_test::component::hand_tracking

#endif

#endif // !__HAND_TRACKING_COMPONENT_H__
