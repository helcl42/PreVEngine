#include "OpenXrInput.h"

#ifdef ENABLE_XR

#include "../XrEvents.h"

#include "../../event/EventChannel.h"
#include "../../util/MathUtils.h"

#include <vector>

namespace prev::xr::input {
namespace {
    XrPath ConvertStringToXrPath(XrInstance instance, const char* pathString)
    {
        XrPath xrPath;
        OPENXR_CHECK(xrStringToPath(instance, pathString, &xrPath), "Failed to create XrPath from string.");
        return xrPath;
    }

    std::string ConvertXrPathToString(XrInstance instance, const XrPath& path)
    {
        uint32_t strl;
        char text[XR_MAX_PATH_LENGTH];
        XrResult res;
        res = xrPathToString(instance, path, XR_MAX_PATH_LENGTH, &strl, text);
        std::string str;
        if (res == XR_SUCCESS) {
            str = text;
        } else {
            OPENXR_CHECK(res, "Failed to retrieve path.");
        }
        return str;
    }
}

OpenXrInput::OpenXrInput(XrInstance instance, XrSystemId systemId)
    : m_instance{ instance }
    , m_systemId{ systemId }
{
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
}

void OpenXrInput::OnSessionDestroy()
{
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
    XrActionSetCreateInfo actionSetCI{ XR_TYPE_ACTION_SET_CREATE_INFO };
    actionSetCI.priority = 0;
    strncpy(actionSetCI.actionSetName, "pre-v-engine-action-set", XR_MAX_ACTION_SET_NAME_SIZE);
    strncpy(actionSetCI.localizedActionSetName, "pre-v-engine-action-set", XR_MAX_LOCALIZED_ACTION_SET_NAME_SIZE);
    OPENXR_CHECK(xrCreateActionSet(m_instance, &actionSetCI, &m_actionSet), "Failed to create ActionSet.");

    auto CreateAction = [this](const char* name, const XrActionType xrActionType, const std::vector<const char*>& subactionPaths = {}) -> XrAction {
        XrActionCreateInfo actionCI{ XR_TYPE_ACTION_CREATE_INFO };
        actionCI.actionType = xrActionType;
        std::vector<XrPath> subactionXrPaths;
        for (const auto& p : subactionPaths) {
            subactionXrPaths.push_back(ConvertStringToXrPath(m_instance, p));
        }
        actionCI.countSubactionPaths = static_cast<uint32_t>(subactionXrPaths.size());
        actionCI.subactionPaths = subactionXrPaths.data();
        // The internal name the runtime uses for this Action.
        strncpy(actionCI.actionName, name, XR_MAX_ACTION_NAME_SIZE);
        // Localized names are required so there is a human-readable action name to show the user if they are rebinding the Action in an options screen.
        strncpy(actionCI.localizedActionName, name, XR_MAX_LOCALIZED_ACTION_NAME_SIZE);

        XrAction xrAction{};
        OPENXR_CHECK(xrCreateAction(m_actionSet, &actionCI, &xrAction), "Failed to create Action.");
        return xrAction;
    };

    // TODO - read this from a file - make a general action mechanism?
    m_squeezeAction = CreateAction("squeeze", XR_ACTION_TYPE_FLOAT_INPUT, { "/user/hand/left", "/user/hand/right" });
    m_triggerAction = CreateAction("trigger", XR_ACTION_TYPE_BOOLEAN_INPUT, { "/user/hand/left", "/user/hand/right" });
    m_palmPoseAction = CreateAction("pose", XR_ACTION_TYPE_POSE_INPUT, { "/user/hand/left", "/user/hand/right" });

    m_handPaths[0] = ConvertStringToXrPath(m_instance, "/user/hand/left");
    m_handPaths[1] = ConvertStringToXrPath(m_instance, "/user/hand/right");
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

    anyOk |= SuggestBindings(m_instance, "/interaction_profiles/oculus/touch_controller", {
        { m_squeezeAction, ConvertStringToXrPath(m_instance, "/user/hand/left/input/squeeze/value") },
        { m_squeezeAction, ConvertStringToXrPath(m_instance, "/user/hand/right/input/squeeze/value") },
        { m_triggerAction, ConvertStringToXrPath(m_instance, "/user/hand/left/input/trigger/value") },
        { m_triggerAction, ConvertStringToXrPath(m_instance, "/user/hand/right/input/trigger/value") },
        { m_palmPoseAction, ConvertStringToXrPath(m_instance, "/user/hand/left/input/grip/pose") },
        { m_palmPoseAction, ConvertStringToXrPath(m_instance, "/user/hand/right/input/grip/pose") }
    });
    return anyOk;
}

void OpenXrInput::CreateActionPoses()
{
    auto CreateActionPoseSpace = [](const XrInstance instance, const XrSession session, const XrAction xrAction, const char* subactionPath = nullptr) -> XrSpace {
        const XrPosef xrPoseIdentity = { { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } };
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

    m_handPoseSpace[0] = CreateActionPoseSpace(m_instance, m_session, m_palmPoseAction, "/user/hand/left");
    m_handPoseSpace[1] = CreateActionPoseSpace(m_instance, m_session, m_palmPoseAction, "/user/hand/right");
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

void OpenXrInput::RecordCurrentBindings()
{
    if (!m_session) {
        return;
    }

    XrInteractionProfileState interactionProfile = { XR_TYPE_INTERACTION_PROFILE_STATE, 0, 0 };
    OPENXR_CHECK(xrGetCurrentInteractionProfile(m_session, m_handPaths[0], &interactionProfile), "Failed to get profile.");
    if (interactionProfile.interactionProfile) {
        LOGI("user/hand/left ActiveProfile: %s", ConvertXrPathToString(m_instance, interactionProfile.interactionProfile).c_str());
    }
    OPENXR_CHECK(xrGetCurrentInteractionProfile(m_session, m_handPaths[1], &interactionProfile), "Failed to get profile.");
    if (interactionProfile.interactionProfile) {
        LOGI("user/hand/right ActiveProfile: %s", ConvertXrPathToString(m_instance, interactionProfile.interactionProfile).c_str());
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

    XrHandsEvent handsEvent{};
    for (int i = 0; i < 2; ++i) {
        auto& handPoseState{ m_handPoseState[i] };
        auto& handPose{ m_handPose[i] };
        auto& squeezeState{ m_squeezeState[i] };
        auto& triggerState{ m_triggerState[i] };

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

        XrActionStateGetInfo squeezeActionStateGetInfo{ XR_TYPE_ACTION_STATE_GET_INFO };
        squeezeActionStateGetInfo.action = m_squeezeAction;
        squeezeActionStateGetInfo.subactionPath = m_handPaths[i];
        OPENXR_CHECK(xrGetActionStateFloat(m_session, &squeezeActionStateGetInfo, &m_squeezeState[i]), "Failed to get Float State of squeeze action.");

        XrActionStateGetInfo triggerActionStateGetInfo{ XR_TYPE_ACTION_STATE_GET_INFO };
        triggerActionStateGetInfo.action = m_triggerAction;
        triggerActionStateGetInfo.subactionPath = m_handPaths[i];
        OPENXR_CHECK(xrGetActionStateBoolean(m_session, &triggerActionStateGetInfo, &m_triggerState[i]), "Failed to get Boolean State of trigger action.");

        auto& handEvent{ handsEvent.hands[i] };
        handEvent.type = i == 0 ? HandType::LEFT : HandType::RIGHT;
        handEvent.active = handPoseState.isActive;
        handEvent.pose = prev::util::math::Pose{ { handPose.orientation.w, handPose.orientation.x, handPose.orientation.y, handPose.orientation.z }, { handPose.position.x, handPose.position.y, handPose.position.z } };
        handEvent.flags = {};
        handEvent.flags |= (squeezeState.isActive == XR_TRUE) ? HandEventFlags::SQUEEZE : HandEventFlags::NONE;
        handEvent.flags |= (triggerState.isActive == XR_TRUE && triggerState.currentState == XR_FALSE && triggerState.changedSinceLastSync == XR_TRUE) ? HandEventFlags::TRIGGER : HandEventFlags::NONE;
        handEvent.squeeze = squeezeState.currentState;
    }

    prev::event::EventChannel::Post(handsEvent);
}
} // namespace prev::xr::input

#endif
