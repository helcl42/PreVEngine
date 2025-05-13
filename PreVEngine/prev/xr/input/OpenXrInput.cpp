#include "OpenXrInput.h"

#ifdef ENABLE_XR

#include "../XrEvents.h"

#include "../../event/EventChannel.h"
#include "../../util/MathUtils.h"

#include <vector>

namespace prev::xr::input {
namespace {
    PFN_xrCreateHandTrackerEXT xrCreateHandTrackerEXT{};
    PFN_xrDestroyHandTrackerEXT xrDestroyHandTrackerEXT{};
    PFN_xrLocateHandJointsEXT xrLocateHandJointsEXT{};

    inline XrPath ConvertStringToXrPath(XrInstance instance, const char* pathString)
    {
        XrPath xrPath;
        OPENXR_CHECK(xrStringToPath(instance, pathString, &xrPath), "Failed to create XrPath from string.");
        return xrPath;
    }

    inline std::string ConvertXrPathToString(XrInstance instance, const XrPath& path)
    {
        uint32_t strLength;
        char text[XR_MAX_PATH_LENGTH];
        OPENXR_CHECK(xrPathToString(instance, path, XR_MAX_PATH_LENGTH, &strLength, text), "Failed to retrieve path.");
        return std::string(text);
    }

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
{
    LoadXrExtensionFunctions(m_instance);

    CreateActionSet();

    SuggestBindings();
}

OpenXrInput::~OpenXrInput()
{
    DestroyActionSet();
}

void OpenXrInput::OnSessionCreate(XrSession session)
{
    m_session = session;

    CreateActionPoses();
    AttachActionSet();
    CreateHandTrackers();
}

void OpenXrInput::OnSessionDestroy()
{
    DestroyHandTrackers();
    DetachActionSet();
    DestroyActionPoses();

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

void OpenXrInput::CreateActionSet()
{
    XrActionSetCreateInfo actionSetCreateInfo{ XR_TYPE_ACTION_SET_CREATE_INFO };
    actionSetCreateInfo.priority = 0;
    strncpy(actionSetCreateInfo.actionSetName, "pre-v-engine-action-set", XR_MAX_ACTION_SET_NAME_SIZE);
    strncpy(actionSetCreateInfo.localizedActionSetName, "pre-v-engine-action-set", XR_MAX_LOCALIZED_ACTION_SET_NAME_SIZE);
    OPENXR_CHECK(xrCreateActionSet(m_instance, &actionSetCreateInfo, &m_actionSet), "Failed to create ActionSet.");

    auto CreateAction = [this](const char* name, const XrActionType xrActionType, const std::vector<const char*>& subactionPaths = {}) -> XrAction {
        XrActionCreateInfo actionCreateInfo{ XR_TYPE_ACTION_CREATE_INFO };
        actionCreateInfo.actionType = xrActionType;
        std::vector<XrPath> subactionXrPaths;
        for (const auto& p : subactionPaths) {
            subactionXrPaths.push_back(ConvertStringToXrPath(m_instance, p));
        }
        actionCreateInfo.countSubactionPaths = static_cast<uint32_t>(subactionXrPaths.size());
        actionCreateInfo.subactionPaths = subactionXrPaths.data();
        // The internal name the runtime uses for this Action.
        strncpy(actionCreateInfo.actionName, name, XR_MAX_ACTION_NAME_SIZE);
        // Localized names are required so there is a human-readable action name to show the user if they are rebinding the Action in an options screen.
        strncpy(actionCreateInfo.localizedActionName, name, XR_MAX_LOCALIZED_ACTION_NAME_SIZE);

        XrAction xrAction{};
        OPENXR_CHECK(xrCreateAction(m_actionSet, &actionCreateInfo, &xrAction), "Failed to create Action.");
        return xrAction;
    };

    // TODO - read this from a file - make a general action mechanism?
    m_squeezeAction = CreateAction("squeeze", XR_ACTION_TYPE_FLOAT_INPUT, { "/user/hand/left", "/user/hand/right" });
    m_triggerAction = CreateAction("trigger", XR_ACTION_TYPE_BOOLEAN_INPUT, { "/user/hand/left", "/user/hand/right" });
    m_palmPoseAction = CreateAction("pose", XR_ACTION_TYPE_POSE_INPUT, { "/user/hand/left", "/user/hand/right" });

    // TODO - unify handPaths(duplicates)
    const char* handPaths[MAX_HAND_COUNT] = {
        "/user/hand/left",
        "/user/hand/right"
    };
    for (uint32_t i = 0; i < MAX_HAND_COUNT; ++i) {
        m_handPaths[i] = ConvertStringToXrPath(m_instance, handPaths[i]);
    }
}

void OpenXrInput::DestroyActionSet()
{
    // nothing to do here ??
}

bool OpenXrInput::SuggestBindings()
{
    auto SuggestBindings = [](const XrInstance instance, const char* profilePath, const std::vector<XrActionSuggestedBinding>& bindings) -> bool {
        XrInteractionProfileSuggestedBinding interactionProfileSuggestedBinding{ XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
        interactionProfileSuggestedBinding.interactionProfile = ConvertStringToXrPath(instance, profilePath);
        interactionProfileSuggestedBinding.suggestedBindings = bindings.data();
        interactionProfileSuggestedBinding.countSuggestedBindings = static_cast<uint32_t>(bindings.size());
        if (xrSuggestInteractionProfileBindings(instance, &interactionProfileSuggestedBinding) == XrResult::XR_SUCCESS) {
            return true;
        }
        LOGI("Failed to suggest bindings with %s", profilePath);
        return false;
    };

    bool anyOk{ false };
    anyOk |= SuggestBindings(m_instance, "/interaction_profiles/khr/simple_controller", {
                                                                                            { m_triggerAction, ConvertStringToXrPath(m_instance, "/user/hand/left/input/select/click") },
                                                                                            { m_squeezeAction, ConvertStringToXrPath(m_instance, "/user/hand/right/input/select/click") },
                                                                                            { m_palmPoseAction, ConvertStringToXrPath(m_instance, "/user/hand/left/input/grip/pose") },
                                                                                            { m_palmPoseAction, ConvertStringToXrPath(m_instance, "/user/hand/right/input/grip/pose") },
                                                                                        });

    anyOk |= SuggestBindings(m_instance, "/interaction_profiles/oculus/touch_controller", { { m_squeezeAction, ConvertStringToXrPath(m_instance, "/user/hand/left/input/squeeze/value") }, { m_squeezeAction, ConvertStringToXrPath(m_instance, "/user/hand/right/input/squeeze/value") }, { m_triggerAction, ConvertStringToXrPath(m_instance, "/user/hand/left/input/trigger/value") }, { m_triggerAction, ConvertStringToXrPath(m_instance, "/user/hand/right/input/trigger/value") }, { m_palmPoseAction, ConvertStringToXrPath(m_instance, "/user/hand/left/input/grip/pose") }, { m_palmPoseAction, ConvertStringToXrPath(m_instance, "/user/hand/right/input/grip/pose") } });
    return anyOk;
}

void OpenXrInput::CreateActionPoses()
{
    auto CreateActionPoseSpace = [](const XrInstance instance, const XrSession session, const XrAction xrAction, const char* subactionPath = nullptr) -> XrSpace {
        const XrPosef xrPoseIdentity{ { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } };
        // Create frame of reference for a pose action
        XrActionSpaceCreateInfo actionSpaceCI{ XR_TYPE_ACTION_SPACE_CREATE_INFO };
        actionSpaceCI.action = xrAction;
        actionSpaceCI.poseInActionSpace = xrPoseIdentity;
        if (subactionPath) {
            actionSpaceCI.subactionPath = ConvertStringToXrPath(instance, subactionPath);
        }

        XrSpace xrSpace{};
        OPENXR_CHECK(xrCreateActionSpace(session, &actionSpaceCI, &xrSpace), "Failed to create ActionSpace.");
        return xrSpace;
    };

    const char* handPaths[MAX_HAND_COUNT] = {
        "/user/hand/left",
        "/user/hand/right"
    };
    for (uint32_t i = 0; i < MAX_HAND_COUNT; ++i) {
        m_handPoseSpace[i] = CreateActionPoseSpace(m_instance, m_session, m_palmPoseAction, handPaths[i]);
    }
}

void OpenXrInput::DestroyActionPoses()
{
    // nothing to do here
}

void OpenXrInput::AttachActionSet()
{
    // Attach the action set we just made to the session. We could attach multiple action sets!
    XrSessionActionSetsAttachInfo actionSetAttachInfo{ XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO };
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

    for (uint32_t i = 0; i < MAX_HAND_COUNT; ++i) {
        auto& hand{ m_hands[i] };
        XrHandTrackerCreateInfoEXT xrHandTrackerCreateInfo = { XR_TYPE_HAND_TRACKER_CREATE_INFO_EXT };
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

    for (uint32_t i = 0; i < MAX_HAND_COUNT; ++i) {
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

    const char* handPaths[MAX_HAND_COUNT] = {
        "/user/hand/left",
        "/user/hand/right"
    };

    for (uint32_t i = 0; i < MAX_HAND_COUNT; ++i) {
        XrInteractionProfileState interactionProfile{ XR_TYPE_INTERACTION_PROFILE_STATE, 0, 0 };
        OPENXR_CHECK(xrGetCurrentInteractionProfile(m_session, m_handPaths[i], &interactionProfile), "Failed to get profile.");
        if (interactionProfile.interactionProfile) {
            LOGI("%s ActiveProfile: %s", handPaths[i], ConvertXrPathToString(m_instance, interactionProfile.interactionProfile).c_str());
        }
    }
}

void OpenXrInput::PollActions(const XrTime time)
{
    XrActiveActionSet activeActionSet{};
    activeActionSet.actionSet = m_actionSet;
    activeActionSet.subactionPath = XR_NULL_PATH;

    XrActionsSyncInfo actionsSyncInfo{ XR_TYPE_ACTIONS_SYNC_INFO };
    actionsSyncInfo.countActiveActionSets = 1;
    actionsSyncInfo.activeActionSets = &activeActionSet;
    OPENXR_CHECK(xrSyncActions(m_session, &actionsSyncInfo), "Failed to sync Actions.");

    HandleControllerActions(time);
    HandleHandTrackingActions(time);
}

void OpenXrInput::OnEvent(const XrEventDataBuffer& evt)
{
    switch (evt.type) {
    case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED: {
        const XrEventDataInteractionProfileChanged* interactionProfileChanged = reinterpret_cast<const XrEventDataInteractionProfileChanged*>(&evt);
        LOGI("OPENXR: Interaction Profile changed for Session: %p", interactionProfileChanged->session);
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

void OpenXrInput::HandleControllerActions(const XrTime time)
{
    XrHandControllersEvent handControllersEvent{};

    for (uint32_t i = 0; i < MAX_HAND_COUNT; ++i) {
        XrActionStatePose handPoseState{ XR_TYPE_ACTION_STATE_POSE };
        XrPosef handPose{ { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } };
        XrActionStateGetInfo poseActionStateGetInfo{ XR_TYPE_ACTION_STATE_GET_INFO };
        poseActionStateGetInfo.action = m_palmPoseAction;
        poseActionStateGetInfo.subactionPath = m_handPaths[i];
        OPENXR_CHECK(xrGetActionStatePose(m_session, &poseActionStateGetInfo, &handPoseState), "Failed to get Pose State.");
        if (handPoseState.isActive) {
            XrSpaceLocation spaceLocation{ XR_TYPE_SPACE_LOCATION };
            XrResult res = xrLocateSpace(m_handPoseSpace[i], m_localSpace, time, &spaceLocation);
            if (XR_UNQUALIFIED_SUCCESS(res) && (spaceLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) != 0 && (spaceLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) != 0) {
                handPose = spaceLocation.pose;
            } else {
                handPoseState.isActive = false;
            }
        }

        XrActionStateFloat squeezeState{ XR_TYPE_ACTION_STATE_FLOAT };
        XrActionStateGetInfo squeezeActionStateGetInfo{ XR_TYPE_ACTION_STATE_GET_INFO };
        squeezeActionStateGetInfo.action = m_squeezeAction;
        squeezeActionStateGetInfo.subactionPath = m_handPaths[i];
        OPENXR_CHECK(xrGetActionStateFloat(m_session, &squeezeActionStateGetInfo, &squeezeState), "Failed to get Float State of squeeze action.");

        XrActionStateBoolean triggerState{ XR_TYPE_ACTION_STATE_BOOLEAN };
        XrActionStateGetInfo triggerActionStateGetInfo{ XR_TYPE_ACTION_STATE_GET_INFO };
        triggerActionStateGetInfo.action = m_triggerAction;
        triggerActionStateGetInfo.subactionPath = m_handPaths[i];
        OPENXR_CHECK(xrGetActionStateBoolean(m_session, &triggerActionStateGetInfo, &triggerState), "Failed to get Boolean State of trigger action.");

        auto& handControllerEvent{ handControllersEvent.handControllers[i] };
        handControllerEvent.type = i == 0 ? HandType::LEFT : HandType::RIGHT;
        handControllerEvent.active = handPoseState.isActive;
        handControllerEvent.pose = prev::util::math::Pose{ { handPose.orientation.w, handPose.orientation.x, handPose.orientation.y, handPose.orientation.z }, { handPose.position.x, handPose.position.y, handPose.position.z } };
        handControllerEvent.flags = {};
        handControllerEvent.flags |= (squeezeState.isActive == XR_TRUE) ? HandEventFlags::SQUEEZE : HandEventFlags::NONE;
        handControllerEvent.flags |= (triggerState.isActive == XR_TRUE && triggerState.currentState == XR_FALSE && triggerState.changedSinceLastSync == XR_TRUE) ? HandEventFlags::TRIGGER : HandEventFlags::NONE;
        handControllerEvent.squeeze = squeezeState.currentState;
    }

    prev::event::EventChannel::Post(handControllersEvent);
}

void OpenXrInput::HandleHandTrackingActions(const XrTime time)
{
    if (!m_handTrackingEnabled) {
        return;
    }

    XrHandsEvent handsEvent{};

    for (uint32_t i = 0; i < MAX_HAND_COUNT; ++i) {
        auto& hand{ m_hands[i] };

        XrHandJointsMotionRangeInfoEXT motionRangeInfo{ XR_TYPE_HAND_JOINTS_MOTION_RANGE_INFO_EXT };
        motionRangeInfo.handJointsMotionRange = XR_HAND_JOINTS_MOTION_RANGE_UNOBSTRUCTED_EXT; // XR_HAND_JOINTS_MOTION_RANGE_CONFORMING_TO_CONTROLLER_EXT

        XrHandJointsLocateInfoEXT locateInfo{ XR_TYPE_HAND_JOINTS_LOCATE_INFO_EXT };
        locateInfo.next = &motionRangeInfo;
        locateInfo.baseSpace = m_localSpace;
        locateInfo.time = time;

        XrHandJointLocationEXT jointLocations[XR_HAND_JOINT_COUNT_EXT]{};
        XrHandJointLocationsEXT locations{ XR_TYPE_HAND_JOINT_LOCATIONS_EXT };
        locations.jointCount = XR_HAND_JOINT_COUNT_EXT;
        locations.jointLocations = jointLocations;
        OPENXR_CHECK(xrLocateHandJointsEXT(hand, &locateInfo, &locations), "Failed to locate hand joints.");

        auto& handEvent{ handsEvent.hands[i] };
        handEvent.type = i == 0 ? HandType::LEFT : HandType::RIGHT;
        handEvent.active = locations.isActive;
        for (uint32_t j = 0; j < locations.jointCount; ++j) {
            const auto& jointLocation{ locations.jointLocations[j] };

            auto& handJoint{ handEvent.joints[j] };
            handJoint.active = (jointLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) != 0 && (jointLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) != 0;
            handJoint.pose =  prev::util::math::Pose{ {  jointLocation.pose.orientation.w,  jointLocation.pose.orientation.x,  jointLocation.pose.orientation.y,  jointLocation.pose.orientation.z }, {  jointLocation.pose.position.x,  jointLocation.pose.position.y,  jointLocation.pose.position.z } };
            handJoint.radius = jointLocation.radius;
        }
    }

    prev::event::EventChannel::Post(handsEvent);
}
} // namespace prev::xr::input

#endif
