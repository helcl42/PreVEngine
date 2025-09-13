#ifndef __OPEN_XR_INPUT_H__
#define __OPEN_XR_INPUT_H__

#ifdef ENABLE_XR

#include "../common/IOpenXrEventObserver.h"

#include "../XrEvents.h"

#include <prev/event/EventHandler.h>

#include <array>
#include <optional>

namespace prev::xr::input {
class OpenXrInput final : public common::IOpenXrEventObserver {
public:
    OpenXrInput(XrInstance instance, XrSystemId systemId, bool handTrackingEnabled);

    ~OpenXrInput();

public:
    void OnSessionCreate(XrSession session);

    void OnSessionDestroy();

    void OnReferenceSpaceCreate(XrSpace space);

    void OnReferenceSpaceDestroy();

public:
    void CreateActionSet();

    void DestroyActionSet();

    void CreateActionPoses();

    void DestroyActionPoses();

    void AttachActionSet();

    void DetachActionSet();

    void CreateHandTrackers();

    void DestroyHandTrackers();

    bool SuggestBindings();

    void RecordCurrentBindings();

    void PollActions(const XrTime time);

public:
    void operator() (const XrHapticFeedback& hapticFeedback);

public:
    void OnEvent(const XrEventDataBuffer& evt) override;

private:
    void HandleControllerActions(const XrTime time);

    void HandleHandTrackingActions(const XrTime time);

private:
    XrInstance m_instance;
    XrSystemId m_systemId;
    bool m_handTrackingEnabled;

    XrSession m_session;
    XrSpace m_localSpace;

    // Controllers
    XrActionSet m_actionSet{};
    XrAction m_squeezeAction{};
    XrAction m_triggerAction{};
    XrAction m_palmPoseAction{};
    XrAction m_quitAction{};
    XrAction m_vibrateAction{};
    XrPath m_handPaths[MAX_HAND_COUNT]{};
    XrSpace m_handPoseSpace[MAX_HAND_COUNT]{};

    XrHandTrackerEXT m_hands[MAX_HAND_COUNT]{};

private:
    prev::event::EventHandler<OpenXrInput, XrHapticFeedback> m_vibrationEventHandler{ *this };

    std::array<std::optional<XrHapticFeedback>, MAX_HAND_COUNT> m_hapticFeedbackEvents;
};
} // namespace prev::xr::input

#endif

#endif