#ifndef __OPEN_XR_INPUT_H__
#define __OPEN_XR_INPUT_H__

#ifdef ENABLE_XR

#include "../common/IOpenXrEventObserver.h"

#include "../../XrEvents.h"

#include <prev/event/EventHandler.h>

#include <array>
#include <optional>

namespace prev::xr::open_xr::input {
class OpenXrInput final : public common::IOpenXrEventObserver {
public:
    OpenXrInput(XrInstance instance, XrSystemId systemId, bool handTrackingEnabled);

    ~OpenXrInput();

public:
    void OnSessionCreate(XrSession session);

    void OnSessionDestroy();

    void OnReferenceSpaceCreate(XrSpace space);

    void OnReferenceSpaceDestroy();

    void PollActions(const XrTime time);

public:
    void OnEvent(const XrEventDataBuffer& evt) override;

public:
    void operator()(const HapticFeedback& hapticFeedback);

private:
    void CreateActionSet();

    void DestroyActionSet();

    void CreateActionSpaces();

    void DestroyActionSpaces();

    void AttachActionSet();

    void DetachActionSet();

    void CreateHandTrackers();

    void DestroyHandTrackers();

    bool SuggestControllerBindings();

    void RecordCurrentBindings();

    void HandleControllerActions(const XrTime time);

    void HandleHandTrackingActions(const XrTime time);

private:
    XrInstance m_instance;
    XrSystemId m_systemId;
    bool m_handTrackingEnabled;

    XrSession m_session;
    XrSpace m_localSpace;

    std::array<const char*, MAX_HAND_COUNT> m_handPathStrings{
        "/user/hand/left",
        "/user/hand/right",
    };
    std::array<XrPath, MAX_HAND_COUNT> m_handPaths{};

    XrActionSet m_actionSet{};

    // Controllers
    XrAction m_squeezeAction{};
    XrAction m_triggerAction{};
    XrAction m_palmPoseAction{};
    XrAction m_quitAction{};
    XrAction m_vibrateAction{};
    std::array<XrSpace, MAX_HAND_COUNT> m_handPoseSpace{};

private:
    prev::event::EventHandler<OpenXrInput, HapticFeedback> m_vibrationEventHandler{ *this };

    std::array<std::optional<HapticFeedback>, MAX_HAND_COUNT> m_hapticFeedbackEvents;

    // HandTracking
    std::array<XrHandTrackerEXT, MAX_HAND_COUNT> m_hands{};
    XrAction m_poseAction{};
    XrAction m_grabAction{};

    std::array<XrSpace, MAX_HAND_COUNT> m_handSpace{};
};
} // namespace prev::xr::open_xr::input

#endif

#endif