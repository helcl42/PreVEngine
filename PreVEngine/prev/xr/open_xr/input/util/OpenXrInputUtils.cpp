#include "OpenXrInputUtils.h"

#ifdef ENABLE_XR

#include "../../util/OpenXrUtils.h"

namespace prev::xr::open_xr::input::util {
XrPath ConvertStringToXrPath(const XrInstance& instance, const char* pathString)
{
    XrPath xrPath;
    OPENXR_CHECK(xrStringToPath(instance, pathString, &xrPath), "Failed to create XrPath from string.");
    return xrPath;
}

std::string ConvertXrPathToString(const XrInstance& instance, const XrPath& path)
{
    uint32_t strLength;
    char text[XR_MAX_PATH_LENGTH];
    OPENXR_CHECK(xrPathToString(instance, path, XR_MAX_PATH_LENGTH, &strLength, text), "Failed to retrieve path.");
    return std::string(text);
}

XrAction CreateAction(const XrActionSet& actionSet, const char* name, const XrActionType xrActionType, const std::vector<XrPath>& subactionPaths)
{
    XrActionCreateInfo actionCreateInfo{ open_xr::util::CreateStruct<XrActionCreateInfo>(XR_TYPE_ACTION_CREATE_INFO) };
    actionCreateInfo.actionType = xrActionType;
    actionCreateInfo.countSubactionPaths = static_cast<uint32_t>(subactionPaths.size());
    actionCreateInfo.subactionPaths = subactionPaths.empty() ? XR_NULL_PATH : subactionPaths.data();
    // The internal name the runtime uses for this Action.
    strncpy(actionCreateInfo.actionName, name, XR_MAX_ACTION_NAME_SIZE);
    // Localized names are required so there is a human-readable action name to show the user if they are rebinding the Action in an options screen.
    strncpy(actionCreateInfo.localizedActionName, name, XR_MAX_LOCALIZED_ACTION_NAME_SIZE);

    XrAction xrAction{};
    OPENXR_CHECK(xrCreateAction(actionSet, &actionCreateInfo, &xrAction), "Failed to create Action.");
    return xrAction;
}

bool SuggestProfileBindings(const XrInstance& instance, const char* profilePath, const std::vector<XrActionSuggestedBinding>& bindings)
{
    XrInteractionProfileSuggestedBinding interactionProfileSuggestedBinding{ open_xr::util::CreateStruct<XrInteractionProfileSuggestedBinding>(XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING) };
    interactionProfileSuggestedBinding.interactionProfile = ConvertStringToXrPath(instance, profilePath);
    interactionProfileSuggestedBinding.suggestedBindings = bindings.data();
    interactionProfileSuggestedBinding.countSuggestedBindings = static_cast<uint32_t>(bindings.size());
    if (xrSuggestInteractionProfileBindings(instance, &interactionProfileSuggestedBinding) != XrResult::XR_SUCCESS) {
        LOGI("Failed to suggest bindings with %s", profilePath);
        return false;
    }
    return true;
};

XrSpace CreateActionSpace(const XrSession& session, const XrAction& xrAction, const XrPath& subactionPath)
{
    const XrPosef xrPoseIdentity{ { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } };
    // Create frame of reference for a pose action
    XrActionSpaceCreateInfo actionSpaceCI{ open_xr::util::CreateStruct<XrActionSpaceCreateInfo>(XR_TYPE_ACTION_SPACE_CREATE_INFO) };
    actionSpaceCI.action = xrAction;
    actionSpaceCI.poseInActionSpace = xrPoseIdentity;
    actionSpaceCI.subactionPath = subactionPath;

    XrSpace xrSpace{};
    OPENXR_CHECK(xrCreateActionSpace(session, &actionSpaceCI, &xrSpace), "Failed to create ActionSpace.");
    return xrSpace;
}

void DestroyActionSpace(XrSpace& space)
{
    OPENXR_CHECK(xrDestroySpace(space), "Failed to destroy ActionSpace.")
    space = {};
}

prev::util::math::Pose ConvertXrPoseToPose(const XrPosef& xrPose)
{
    return prev::util::math::Pose{
        { xrPose.orientation.w, xrPose.orientation.x, xrPose.orientation.y, xrPose.orientation.z },
        { xrPose.position.x, xrPose.position.y, xrPose.position.z },
    };
}

XrPosef ConvertPoseToXrPose(const prev::util::math::Pose& pose)
{
    return XrPosef{
        { pose.orientation.x, pose.orientation.y, pose.orientation.z, pose.orientation.w },
        { pose.position.x, pose.position.y, pose.position.z }
    };
}

std::optional<prev::util::math::Pose> GetPoseState(const XrSession& session, const XrTime& time, const XrAction& action, const XrPath& subActionPath, const XrSpace& baseSpace, const XrSpace& space)
{
    XrActionStatePose poseState{ open_xr::util::CreateStruct<XrActionStatePose>(XR_TYPE_ACTION_STATE_POSE) };
    XrActionStateGetInfo poseActionStateGetInfo{ open_xr::util::CreateStruct<XrActionStateGetInfo>(XR_TYPE_ACTION_STATE_GET_INFO) };
    poseActionStateGetInfo.action = action;
    poseActionStateGetInfo.subactionPath = subActionPath;
    OPENXR_CHECK(xrGetActionStatePose(session, &poseActionStateGetInfo, &poseState), "Failed to get Pose state.");
    if (poseState.isActive) {
        XrSpaceLocation spaceLocation{ open_xr::util::CreateStruct<XrSpaceLocation>(XR_TYPE_SPACE_LOCATION) };
        XrResult res = xrLocateSpace(space, baseSpace, time, &spaceLocation);
        if (XR_UNQUALIFIED_SUCCESS(res) && (spaceLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) != 0 && (spaceLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) != 0) {
            return { ConvertXrPoseToPose(spaceLocation.pose) };
        }
    }
    return {};
}

std::optional<float> GetFloatState(const XrSession& session, const XrAction& action, const XrPath& subActionPath)
{
    XrActionStateFloat floatState{ open_xr::util::CreateStruct<XrActionStateFloat>(XR_TYPE_ACTION_STATE_FLOAT) };
    XrActionStateGetInfo floatActionStateGetInfo{ open_xr::util::CreateStruct<XrActionStateGetInfo>(XR_TYPE_ACTION_STATE_GET_INFO) };
    floatActionStateGetInfo.action = action;
    floatActionStateGetInfo.subactionPath = subActionPath;
    OPENXR_CHECK(xrGetActionStateFloat(session, &floatActionStateGetInfo, &floatState), "Failed to get Float state.");
    if (floatState.isActive) {
        return { floatState.currentState };
    }
    return {};
}

std::optional<XrVector2f> GetVector2State(const XrSession& session, const XrAction& action, const XrPath& subActionPath)
{
    XrActionStateVector2f vector2State{ open_xr::util::CreateStruct<XrActionStateVector2f>(XR_TYPE_ACTION_STATE_VECTOR2F) };
    XrActionStateGetInfo vector2ActionStateGetInfo{ open_xr::util::CreateStruct<XrActionStateGetInfo>(XR_TYPE_ACTION_STATE_GET_INFO) };
    vector2ActionStateGetInfo.action = action;
    vector2ActionStateGetInfo.subactionPath = subActionPath;
    OPENXR_CHECK(xrGetActionStateVector2f(session, &vector2ActionStateGetInfo, &vector2State), "Failed to get Vector2 state.");
    if (vector2State.isActive) {
        return { vector2State.currentState };
    }
    return {};
}

std::optional<bool> GetBoolState(const XrSession& session, const XrAction& action, const bool invert, const XrPath& subActionPath)
{
    XrActionStateBoolean boolState{ open_xr::util::CreateStruct<XrActionStateBoolean>(XR_TYPE_ACTION_STATE_BOOLEAN) };
    XrActionStateGetInfo boolActionStateGetInfo{ open_xr::util::CreateStruct<XrActionStateGetInfo>(XR_TYPE_ACTION_STATE_GET_INFO) };
    boolActionStateGetInfo.action = action;
    boolActionStateGetInfo.subactionPath = subActionPath;
    OPENXR_CHECK(xrGetActionStateBoolean(session, &boolActionStateGetInfo, &boolState), "Failed to get Boolean state.");
    if (boolState.isActive && boolState.currentState == !invert && boolState.changedSinceLastSync == XR_TRUE) {
        return { true };
    }
    return {};
}
} // namespace prev::xr::open_xr::input::util

#endif