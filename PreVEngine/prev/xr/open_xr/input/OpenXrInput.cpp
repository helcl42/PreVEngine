#include "OpenXrInput.h"

#ifdef ENABLE_XR

#include "util/OpenXrInputUtils.h"

#include "../util/OpenXrUtils.h"

#include "../../../event/EventChannel.h"
#include "../../../util/MathUtils.h"

#include <vector>

namespace prev::xr::open_xr::input {
namespace {
    PFN_xrCreateHandTrackerEXT xrCreateHandTrackerEXT{};
    PFN_xrDestroyHandTrackerEXT xrDestroyHandTrackerEXT{};
    PFN_xrLocateHandJointsEXT xrLocateHandJointsEXT{};

    inline void LoadXrExtensionFunctions(XrInstance xrInstance)
    {
        OPENXR_CHECK(xrGetInstanceProcAddr(xrInstance, "xrCreateHandTrackerEXT", (PFN_xrVoidFunction*)&xrCreateHandTrackerEXT), "Failed to get xrCreateHandTrackerEXT.");
        OPENXR_CHECK(xrGetInstanceProcAddr(xrInstance, "xrDestroyHandTrackerEXT", (PFN_xrVoidFunction*)&xrDestroyHandTrackerEXT), "Failed to get xrDestroyHandTrackerEXT.");
        OPENXR_CHECK(xrGetInstanceProcAddr(xrInstance, "xrLocateHandJointsEXT", (PFN_xrVoidFunction*)&xrLocateHandJointsEXT), "Failed to get xrLocateHandJointsEXT.");
    }
} // namespace

OpenXrInput::OpenXrInput(XrInstance instance, XrSystemId systemId, bool handTrackingEnabled)
    : m_instance{ instance }
    , m_systemId{ systemId }
    , m_handTrackingEnabled{ handTrackingEnabled }
    , m_session{ XR_NULL_HANDLE }
    , m_localSpace{ XR_NULL_HANDLE }
{
    LoadXrExtensionFunctions(m_instance);

    CreateActionSet();

    SuggestControllerBindings();
}

OpenXrInput::~OpenXrInput()
{
    DestroyActionSet();
}

void OpenXrInput::OnSessionCreate(XrSession session)
{
    m_session = session;

    CreateActionSpaces();
    AttachActionSet();
    CreateHandTrackers();
}

void OpenXrInput::OnSessionDestroy()
{
    DestroyHandTrackers();
    DetachActionSet();
    DestroyActionSpaces();

    m_session = XR_NULL_HANDLE;
}

void OpenXrInput::OnReferenceSpaceCreate(XrSpace space)
{
    m_localSpace = space;
}

void OpenXrInput::OnReferenceSpaceDestroy()
{
    m_localSpace = XR_NULL_HANDLE;
}

void OpenXrInput::PollActions(const XrTime time)
{
    XrActiveActionSet activeActionSet{};
    activeActionSet.actionSet = m_actionSet;
    activeActionSet.subactionPath = XR_NULL_PATH;

    XrActionsSyncInfo actionsSyncInfo{ open_xr::util::CreateStruct<XrActionsSyncInfo>(XR_TYPE_ACTIONS_SYNC_INFO) };
    actionsSyncInfo.countActiveActionSets = 1;
    actionsSyncInfo.activeActionSets = &activeActionSet;
    OPENXR_CHECK(xrSyncActions(m_session, &actionsSyncInfo), "Failed to sync Actions.");

    HandleControllerActions(time);
    HandleHandTrackingActions(time);
}

void OpenXrInput::operator()(const XrHapticFeedback& hapticFeedback)
{
    const auto handIndex{ open_xr::input::util::ConvertHandTypeToIndex<uint32_t>(hapticFeedback.type) };
    m_hapticFeedbackEvents[handIndex] = { hapticFeedback.type, hapticFeedback.amplitude, hapticFeedback.duration };
}

void OpenXrInput::OnEvent(const XrEventDataBuffer& evt)
{
    switch (evt.type) {
    case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED: {
        const XrEventDataInteractionProfileChanged* interactionProfileChanged = reinterpret_cast<const XrEventDataInteractionProfileChanged*>(&evt);
        LOGI("OPENXR: Interaction Profile changed for Session: %p", static_cast<void*>(interactionProfileChanged->session));
        if (interactionProfileChanged->session != m_session) {
            LOGW("XrEventDataInteractionProfileChanged for unknown Session");
            break;
        }
        RecordCurrentBindings();
        break;
    }
    default: {
        break;
    }
    }
}

void OpenXrInput::CreateActionSet()
{
    XrActionSetCreateInfo actionSetCreateInfo{ open_xr::util::CreateStruct<XrActionSetCreateInfo>(XR_TYPE_ACTION_SET_CREATE_INFO) };
    actionSetCreateInfo.priority = 0;
    strncpy(actionSetCreateInfo.actionSetName, "prev-engine-action-set", XR_MAX_ACTION_SET_NAME_SIZE);
    strncpy(actionSetCreateInfo.localizedActionSetName, "prev-engine-action-set", XR_MAX_LOCALIZED_ACTION_SET_NAME_SIZE);
    OPENXR_CHECK(xrCreateActionSet(m_instance, &actionSetCreateInfo, &m_actionSet), "Failed to create ActionSet.");

    for (size_t i = 0; i < m_handPathStrings.size(); ++i) {
        m_handPaths[i] = open_xr::input::util::ConvertStringToXrPath(m_instance, m_handPathStrings[i]);
    }

    // Controllers
    m_squeezeAction = open_xr::input::util::CreateAction(m_actionSet, "squeeze", XR_ACTION_TYPE_FLOAT_INPUT, { m_handPaths.begin(), m_handPaths.end() });
    m_triggerAction = open_xr::input::util::CreateAction(m_actionSet, "trigger", XR_ACTION_TYPE_BOOLEAN_INPUT, { m_handPaths.begin(), m_handPaths.end() });
    m_palmPoseAction = open_xr::input::util::CreateAction(m_actionSet, "pose", XR_ACTION_TYPE_POSE_INPUT, { m_handPaths.begin(), m_handPaths.end() });
    m_quitAction = open_xr::input::util::CreateAction(m_actionSet, "quit", XR_ACTION_TYPE_BOOLEAN_INPUT, { m_handPaths.begin(), m_handPaths.end() });
    m_vibrateAction = open_xr::input::util::CreateAction(m_actionSet, "vibrate", XR_ACTION_TYPE_VIBRATION_OUTPUT, { m_handPaths.begin(), m_handPaths.end() });

    // HandsTracking
    m_poseAction = open_xr::input::util::CreateAction(m_actionSet, "hand_pose", XR_ACTION_TYPE_POSE_INPUT, { m_handPaths.begin(), m_handPaths.end() });
    m_grabAction = open_xr::input::util::CreateAction(m_actionSet, "hand_grab", XR_ACTION_TYPE_FLOAT_INPUT, { m_handPaths.begin(), m_handPaths.end() });
}

void OpenXrInput::DestroyActionSet()
{
    m_grabAction = {};
    m_poseAction = {};

    m_vibrateAction = {};
    m_quitAction = {};
    m_palmPoseAction = {};
    m_triggerAction = {};
    m_squeezeAction = {};

    m_handPaths = {};

    OPENXR_CHECK(xrDestroyActionSet(m_actionSet), "Failed to destroy ActionSet.");
    m_actionSet = {};
}

bool OpenXrInput::SuggestControllerBindings()
{
    bool anyOk{ false };
    // clang-format off
    anyOk |= open_xr::input::util::SuggestProfileBindings(m_instance, "/interaction_profiles/khr/simple_controller", {
        { m_triggerAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/left/input/select/click") },
        { m_squeezeAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/right/input/select/click") },
        { m_palmPoseAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/left/input/grip/pose") },
        { m_palmPoseAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/right/input/grip/pose") },
        { m_quitAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/left/input/menu/click") },
        { m_quitAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/right/input/menu/click") },
        { m_vibrateAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/left/output/haptic") },
        { m_vibrateAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/right/output/haptic") },
    });
    // clang-format on
    // clang-format off
    anyOk |= open_xr::input::util::SuggestProfileBindings(m_instance, "/interaction_profiles/oculus/touch_controller", {
        { m_squeezeAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/left/input/squeeze/value") },
        { m_squeezeAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/right/input/squeeze/value") },
        { m_triggerAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/left/input/trigger/value") },
        { m_triggerAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/right/input/trigger/value") },
        { m_palmPoseAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/left/input/grip/pose") },
        { m_palmPoseAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/right/input/grip/pose") },
        { m_quitAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/left/input/menu/click") },
        { m_vibrateAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/left/output/haptic") },
        { m_vibrateAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/right/output/haptic") },
    });
    // clang-format on
    // clang-format off
    anyOk |= open_xr::input::util::SuggestProfileBindings(m_instance, "/interaction_profiles/htc/vive_controller", {
        { m_squeezeAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/left/input/trigger/value") },
        { m_squeezeAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/right/input/trigger/value") },
        { m_palmPoseAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/left/input/grip/pose") },
        { m_palmPoseAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/right/input/grip/pose") },
        { m_quitAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/left/input/menu/click") },
        { m_quitAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/right/input/menu/click") },
        { m_vibrateAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/left/output/haptic") },
        { m_vibrateAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/right/output/haptic") },
    });
    // clang-format on
    // clang-format off
    anyOk |= open_xr::input::util::SuggestProfileBindings(m_instance, "/interaction_profiles/valve/index_controller", {
        { m_squeezeAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/left/input/squeeze/value") },
        { m_squeezeAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/right/input/squeeze/value") },
        { m_palmPoseAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/left/input/grip/pose") },
        { m_palmPoseAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/right/input/grip/pose") },
        { m_quitAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/left/input/b/click") },
        { m_quitAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/right/input/b/click") },
        { m_vibrateAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/left/output/haptic") },
        { m_vibrateAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/right/output/haptic") },
    });
    // clang-format on
    // clang-format off
    anyOk |= open_xr::input::util::SuggestProfileBindings(m_instance, "/interaction_profiles/microsoft/motion_controller", {
        { m_squeezeAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/left/input/trigger/value") },
        { m_squeezeAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/right/input/trigger/value") },
        { m_palmPoseAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/left/input/grip/pose") },
        { m_palmPoseAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/right/input/grip/pose") },
        { m_quitAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/left/input/menu/click") },
        { m_quitAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/right/input/menu/click") },
        { m_vibrateAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/left/output/haptic") },
        { m_vibrateAction, open_xr::input::util::ConvertStringToXrPath(m_instance, "/user/hand/right/output/haptic") },
    });
    // clang-format on
    return anyOk;
}

void OpenXrInput::CreateActionSpaces()
{
    for (size_t i = 0; i < m_handPaths.size(); ++i) {
        m_handPoseSpace[i] = open_xr::input::util::CreateActionSpace(m_session, m_palmPoseAction, m_handPaths[i]);
    }

    for (size_t i = 0; i < m_handPaths.size(); ++i) {
        m_handSpace[i] = open_xr::input::util::CreateActionSpace(m_session, m_poseAction, m_handPaths[i]);
    }
}

void OpenXrInput::DestroyActionSpaces()
{
    for (size_t i = 0; i < m_handPaths.size(); ++i) {
        open_xr::input::util::DestroyActionSpace(m_handSpace[i]);
    }

    for (size_t i = 0; i < m_handPaths.size(); ++i) {
        open_xr::input::util::DestroyActionSpace(m_handPoseSpace[i]);
    }
}

void OpenXrInput::AttachActionSet()
{
    // Attach the action set we just made to the session. We could attach multiple action sets!
    XrSessionActionSetsAttachInfo actionSetAttachInfo{ open_xr::util::CreateStruct<XrSessionActionSetsAttachInfo>(XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO) };
    actionSetAttachInfo.countActionSets = 1;
    actionSetAttachInfo.actionSets = &m_actionSet;
    OPENXR_CHECK(xrAttachSessionActionSets(m_session, &actionSetAttachInfo), "Failed to attach ActionSet to Session.");
}

void OpenXrInput::DetachActionSet()
{
    // nothing to do here
}

void OpenXrInput::CreateHandTrackers()
{
    if (!m_handTrackingEnabled) {
        LOGI("Hand tracking is not supported.");
        return;
    }

    for (size_t i = 0; i < m_handPaths.size(); ++i) {
        auto& hand{ m_hands[i] };
        XrHandTrackerCreateInfoEXT xrHandTrackerCreateInfo{ open_xr::util::CreateStruct<XrHandTrackerCreateInfoEXT>(XR_TYPE_HAND_TRACKER_CREATE_INFO_EXT) };
        xrHandTrackerCreateInfo.hand = i == 0 ? XR_HAND_LEFT_EXT : XR_HAND_RIGHT_EXT;
        xrHandTrackerCreateInfo.handJointSet = XR_HAND_JOINT_SET_DEFAULT_EXT;
        OPENXR_CHECK(xrCreateHandTrackerEXT(m_session, &xrHandTrackerCreateInfo, &hand), "Failed to create Hand Tracker.");
    }
}

void OpenXrInput::DestroyHandTrackers()
{
    if (!m_handTrackingEnabled) {
        return;
    }

    for (size_t i = 0; i < m_handPaths.size(); ++i) {
        auto& hand{ m_hands[i] };
        OPENXR_CHECK(xrDestroyHandTrackerEXT(hand), "Failed to destroy Hand Tracker.");
        hand = {};
    }
}

void OpenXrInput::RecordCurrentBindings()
{
    if (!m_session) {
        return;
    }

    for (size_t i = 0; i < m_handPaths.size(); ++i) {
        XrInteractionProfileState interactionProfile{ open_xr::util::CreateStruct<XrInteractionProfileState>(XR_TYPE_INTERACTION_PROFILE_STATE) };
        OPENXR_CHECK(xrGetCurrentInteractionProfile(m_session, m_handPaths[i], &interactionProfile), "Failed to get profile.");
        if (interactionProfile.interactionProfile) {
            LOGI("%s ActiveProfile: %s", m_handPathStrings[i], open_xr::input::util::ConvertXrPathToString(m_instance, interactionProfile.interactionProfile).c_str());
        }
    }
}

void OpenXrInput::HandleControllerActions(const XrTime time)
{
    // input events
    XrHandControllersEvent handControllersEvent{};
    for (size_t i = 0; i < m_handPaths.size(); ++i) {
        const auto handPose{ open_xr::input::util::GetPoseState(m_session, time, m_palmPoseAction, m_handPaths[i], m_localSpace, m_handPoseSpace[i]) };
        const auto squeeze{ open_xr::input::util::GetFloatState(m_session, m_squeezeAction, m_handPaths[i]) };
        const auto trigger{ open_xr::input::util::GetFloatState(m_session, m_squeezeAction, m_handPaths[i]) };

        auto& handControllerEvent{ handControllersEvent.handControllers[i] };
        handControllerEvent.type = open_xr::input::util::ConvertIndexToHandType(i);
        handControllerEvent.active = handPose.has_value();
        handControllerEvent.pose = handPose ? *handPose : prev::util::math::Pose{};
        handControllerEvent.flags = {};
        handControllerEvent.flags |= squeeze ? HandEventFlags::SQUEEZE : HandEventFlags::NONE;
        handControllerEvent.flags |= trigger ? HandEventFlags::TRIGGER : HandEventFlags::NONE;
        handControllerEvent.squeeze = squeeze ? *squeeze : 1.0f;
    }
    prev::event::EventChannel::Post(handControllersEvent);

    const auto quit{ open_xr::input::util::GetBoolState(m_session, m_quitAction, true, XR_NULL_PATH) };
    if (quit) {
        // TODO - should we just generate quit event or request exit session direcly?
        xrRequestExitSession(m_session);
    }

    // output events
    for (size_t i = 0; i < m_handPaths.size(); ++i) {
        const auto& hapticFeedbackEvent{ m_hapticFeedbackEvents[i] };
        if (hapticFeedbackEvent) {
            XrHapticVibration hapticFeedback{ open_xr::util::CreateStruct<XrHapticVibration>(XR_TYPE_HAPTIC_VIBRATION) };
            hapticFeedback.amplitude = hapticFeedbackEvent->amplitude;
            hapticFeedback.duration = hapticFeedbackEvent->duration;

            XrHapticActionInfo hapticAction{ open_xr::util::CreateStruct<XrHapticActionInfo>(XR_TYPE_HAPTIC_ACTION_INFO) };
            hapticAction.action = m_vibrateAction;
            hapticAction.subactionPath = m_handPaths[i];
            OPENXR_CHECK(xrApplyHapticFeedback(m_session, &hapticAction, reinterpret_cast<XrHapticBaseHeader*>(&hapticFeedback)), "Failed to apply happtic feedback action.");
        }
        m_hapticFeedbackEvents[i] = {};
    }
}

void OpenXrInput::HandleHandTrackingActions(const XrTime time)
{
    if (!m_handTrackingEnabled) {
        return;
    }

    XrHandsEvent handsEvent{};
    for (size_t i = 0; i < m_handPaths.size(); ++i) {
        const auto& hand{ m_hands[i] };

        const auto handPose{ open_xr::input::util::GetPoseState(m_session, time, m_poseAction, m_handPaths[i], m_localSpace, m_handSpace[i]) };
        const auto squeeze{ open_xr::input::util::GetFloatState(m_session, m_grabAction, m_handPaths[i]) };

        XrHandJointsMotionRangeInfoEXT motionRangeInfo{ open_xr::util::CreateStruct<XrHandJointsMotionRangeInfoEXT>(XR_TYPE_HAND_JOINTS_MOTION_RANGE_INFO_EXT) };
        motionRangeInfo.handJointsMotionRange = XR_HAND_JOINTS_MOTION_RANGE_UNOBSTRUCTED_EXT; // XR_HAND_JOINTS_MOTION_RANGE_CONFORMING_TO_CONTROLLER_EXT

        XrHandJointsLocateInfoEXT locateInfo{ open_xr::util::CreateStruct<XrHandJointsLocateInfoEXT>(XR_TYPE_HAND_JOINTS_LOCATE_INFO_EXT) };
        locateInfo.next = &motionRangeInfo;
        locateInfo.baseSpace = m_localSpace;
        locateInfo.time = time;

        XrHandJointLocationEXT jointLocations[XR_HAND_JOINT_COUNT_EXT]{};
        XrHandJointLocationsEXT locations{ open_xr::util::CreateStruct<XrHandJointLocationsEXT>(XR_TYPE_HAND_JOINT_LOCATIONS_EXT) };
        locations.jointCount = XR_HAND_JOINT_COUNT_EXT;
        locations.jointLocations = jointLocations;
        OPENXR_CHECK(xrLocateHandJointsEXT(hand, &locateInfo, &locations), "Failed to locate hand joints.");

        auto& handEvent{ handsEvent.hands[i] };
        handEvent.type = open_xr::input::util::ConvertIndexToHandType(i);
        handEvent.active = locations.isActive;
        handEvent.pose = handPose ? *handPose : prev::util::math::Pose{};
        handEvent.squeeze = squeeze ? *squeeze : 1.0f;
        for (uint32_t j = 0; j < locations.jointCount; ++j) {
            const auto& jointLocation{ locations.jointLocations[j] };
            auto& handJoint{ handEvent.joints[j] };
            handJoint.type = static_cast<HandJointType>(j);            
            handJoint.active = (jointLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) != 0 && (jointLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) != 0;
            handJoint.pose = open_xr::input::util::ConvertXrPoseToPose(jointLocation.pose);
            handJoint.radius = jointLocation.radius;
        }
    }
    prev::event::EventChannel::Post(handsEvent);
}
} // namespace prev::xr::open_xr::input

#endif
