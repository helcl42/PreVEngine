#include "InputUtils.h"

#ifdef ENABLE_XR

#include "../../util/OpenXrUtils.h"

namespace prev::xr::input::util {
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
    XrActionCreateInfo actionCreateInfo{ prev::xr::util::CreateStruct<XrActionCreateInfo>(XR_TYPE_ACTION_CREATE_INFO) };
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
    XrInteractionProfileSuggestedBinding interactionProfileSuggestedBinding{ prev::xr::util::CreateStruct<XrInteractionProfileSuggestedBinding>(XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING) };
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
    XrActionSpaceCreateInfo actionSpaceCI{ prev::xr::util::CreateStruct<XrActionSpaceCreateInfo>(XR_TYPE_ACTION_SPACE_CREATE_INFO) };
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
    XrActionStatePose poseState{ prev::xr::util::CreateStruct<XrActionStatePose>(XR_TYPE_ACTION_STATE_POSE) };
    XrActionStateGetInfo poseActionStateGetInfo{ prev::xr::util::CreateStruct<XrActionStateGetInfo>(XR_TYPE_ACTION_STATE_GET_INFO) };
    poseActionStateGetInfo.action = action;
    poseActionStateGetInfo.subactionPath = subActionPath;
    OPENXR_CHECK(xrGetActionStatePose(session, &poseActionStateGetInfo, &poseState), "Failed to get Pose state.");
    if (poseState.isActive) {
        XrSpaceLocation spaceLocation{ prev::xr::util::CreateStruct<XrSpaceLocation>(XR_TYPE_SPACE_LOCATION) };
        XrResult res = xrLocateSpace(space, baseSpace, time, &spaceLocation);
        if (XR_UNQUALIFIED_SUCCESS(res) && (spaceLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) != 0 && (spaceLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) != 0) {
            return { ConvertXrPoseToPose(spaceLocation.pose) };
        }
    }
    return {};
}

std::optional<float> GetFloatState(const XrSession& session, const XrAction& action, const XrPath& subActionPath)
{
    XrActionStateFloat floatState{ prev::xr::util::CreateStruct<XrActionStateFloat>(XR_TYPE_ACTION_STATE_FLOAT) };
    XrActionStateGetInfo floatActionStateGetInfo{ prev::xr::util::CreateStruct<XrActionStateGetInfo>(XR_TYPE_ACTION_STATE_GET_INFO) };
    floatActionStateGetInfo.action = action;
    floatActionStateGetInfo.subactionPath = subActionPath;
    OPENXR_CHECK(xrGetActionStateFloat(session, &floatActionStateGetInfo, &floatState), "Failed to get Float state.");
    if (floatState.isActive) {
        return { floatState.currentState };
    }
    return {};
}

std::optional<bool> GetBoolState(const XrSession& session, const XrAction& action, const bool invert, const XrPath& subActionPath)
{
    XrActionStateBoolean boolState{ prev::xr::util::CreateStruct<XrActionStateBoolean>(XR_TYPE_ACTION_STATE_BOOLEAN) };
    XrActionStateGetInfo boolActionStateGetInfo{ prev::xr::util::CreateStruct<XrActionStateGetInfo>(XR_TYPE_ACTION_STATE_GET_INFO) };
    boolActionStateGetInfo.action = action;
    boolActionStateGetInfo.subactionPath = subActionPath;
    OPENXR_CHECK(xrGetActionStateBoolean(session, &boolActionStateGetInfo, &boolState), "Failed to get Boolean state.");
    if (boolState.isActive && boolState.currentState == !invert && boolState.changedSinceLastSync == XR_TRUE) {
        return { true };
    }
    return {};
}

glm::vec3 GetJointPosition(const XrHandJointLocationsEXT& joints, const XrHandJointEXT jointId)
{
    const auto& joint{ joints.jointLocations[jointId] };
    const auto jointPosition{ glm::vec3{ joint.pose.position.x, joint.pose.position.y, joint.pose.position.z } };
    return jointPosition;
}

float GetJointsDistance(const XrHandJointLocationsEXT& joints, const XrHandJointEXT jointId1, const XrHandJointEXT jointId2)
{
    const auto joint1Position{ GetJointPosition(joints, jointId1) };
    const auto joint2Position{ GetJointPosition(joints, jointId2) };
    const auto jointDistance{ glm::distance(joint1Position, joint2Position) };
    return jointDistance;
}

bool DetectPinchAction(const HandType hand, const XrHandJointLocationsEXT& joints, XrHandActionEvent& outPinchAction)
{
    const bool handValid{ joints.isActive == XR_TRUE && joints.jointCount == XR_HAND_JOINT_COUNT_EXT && joints.jointLocations[0].pose.position.x != 0.0f };

    const float tipTipDistance{ GetJointsDistance(joints, XR_HAND_JOINT_THUMB_TIP_EXT, XR_HAND_JOINT_INDEX_TIP_EXT) };
    const float pinchThreshold{ 0.03f }; // 3 cm

    outPinchAction = {};
    outPinchAction.type = hand;
    outPinchAction.actionType = HandActionType::PINCH;
    if (tipTipDistance > pinchThreshold || !handValid) {
        return false;
    }
    const glm::quat pinchOrientation{ 1.0f, 0.0f, 0.0f, 0.0f };
    const glm::vec3 pinchPosition{ (GetJointPosition(joints, XR_HAND_JOINT_THUMB_TIP_EXT) + GetJointPosition(joints, XR_HAND_JOINT_INDEX_TIP_EXT)) / 2.0f };
    outPinchAction.active = true;
    outPinchAction.pose = prev::util::math::Pose{ pinchOrientation, pinchPosition };
    outPinchAction.value = 1.0f - std::min(1.0f, tipTipDistance);
    return true;
}

bool DetectAimAction(const HandType hand, const XrHandJointLocationsEXT& joints, XrHandActionEvent& outPinchAction)
{
    const bool handValid{ joints.isActive == XR_TRUE && joints.jointCount == XR_HAND_JOINT_COUNT_EXT && joints.jointLocations[0].pose.position.x != 0.0f };

    // Check if the index finger is extended.
    const float extendedThreshold{ 0.1f }; // 10 cm

    const auto indexDistance{ GetJointsDistance(joints, XR_HAND_JOINT_INDEX_METACARPAL_EXT, XR_HAND_JOINT_INDEX_TIP_EXT) };
    const auto indexScore{ std::clamp(indexDistance / extendedThreshold, 0.0f, 1.0f) };

    // Check if the other fingers are curled.
    // We check if the distance from each finger tip to its base is small.
    const float curledThreshold{ 0.1f }; // 10 cm

    const auto middleDistance{ GetJointsDistance(joints, XR_HAND_JOINT_MIDDLE_METACARPAL_EXT, XR_HAND_JOINT_MIDDLE_TIP_EXT) };
    const auto middleScore{ 1.0f - std::clamp(middleDistance / curledThreshold, 0.0f, 1.0f) };

    const auto ringDistance{ GetJointsDistance(joints, XR_HAND_JOINT_RING_METACARPAL_EXT, XR_HAND_JOINT_RING_TIP_EXT) };
    const auto ringScore{ 1.0f - std::clamp(ringDistance / curledThreshold, 0.0f, 1.0f) };

    const auto littleDistance{ GetJointsDistance(joints, XR_HAND_JOINT_LITTLE_METACARPAL_EXT, XR_HAND_JOINT_LITTLE_TIP_EXT) };
    const auto littleScore{ 1.0f - std::clamp(littleDistance / curledThreshold, 0.0f, 1.0f) };

    const auto averageCurlScore{ (middleScore + ringScore + littleScore) / 3.0f };

    const float aimActivationThreshold{ 0.6f };
    const auto aimConfidence{ indexScore * averageCurlScore };
    const bool aimActive{ aimConfidence > aimActivationThreshold };

    outPinchAction = {};
    outPinchAction.type = hand;
    outPinchAction.actionType = HandActionType::AIM;
    if (!aimActive || !handValid) {
        return false;
    }

    const glm::vec3 aimDirection(glm::normalize(GetJointPosition(joints, XR_HAND_JOINT_INDEX_TIP_EXT) - GetJointPosition(joints, XR_HAND_JOINT_INDEX_METACARPAL_EXT)));
    const glm::vec3 forwardVector(0.0f, 0.0f, -1.0f);

    const glm::quat aimOrientation{ prev::util::math::CreateQuaternion(forwardVector, aimDirection) };
    const glm::vec3 aimPosition{ GetJointPosition(joints, XR_HAND_JOINT_INDEX_METACARPAL_EXT) };

    outPinchAction.pose = prev::util::math::Pose{ aimOrientation, aimPosition };
    outPinchAction.active = true;
    outPinchAction.value = aimConfidence;

    return true;
}

bool DetectPokeAction(const HandType hand, const XrHandJointLocationsEXT& joints, XrHandActionEvent& outPinchAction)
{
    const bool handValid{ joints.isActive == XR_TRUE && joints.jointCount == XR_HAND_JOINT_COUNT_EXT && joints.jointLocations[0].pose.position.x != 0.0f };

    const float extendedThreshold{ 0.1f }; // 10 cm

    const auto indexDistance{ GetJointsDistance(joints, XR_HAND_JOINT_INDEX_METACARPAL_EXT, XR_HAND_JOINT_INDEX_TIP_EXT) };
    const auto indexScore{ std::clamp(indexDistance / extendedThreshold, 0.0f, 1.0f) };

    const float curledThreshold{ 0.15f }; // 15 cm

    const auto middleDistance{ GetJointsDistance(joints, XR_HAND_JOINT_MIDDLE_METACARPAL_EXT, XR_HAND_JOINT_MIDDLE_TIP_EXT) };
    const auto middleScore{ 1.0f - std::clamp(middleDistance / curledThreshold, 0.0f, 1.0f) };

    const auto ringDistance{ GetJointsDistance(joints, XR_HAND_JOINT_RING_METACARPAL_EXT, XR_HAND_JOINT_RING_TIP_EXT) };
    const auto ringScore{ 1.0f - std::clamp(ringDistance / curledThreshold, 0.0f, 1.0f) };

    const auto littleDistance{ GetJointsDistance(joints, XR_HAND_JOINT_LITTLE_METACARPAL_EXT, XR_HAND_JOINT_LITTLE_TIP_EXT) };
    const auto littleScore{ 1.0f - std::clamp(littleDistance / curledThreshold, 0.0f, 1.0f) };

    const auto averageCurlScore{ (middleScore + ringScore + littleScore) / 3.0f };

    const float curledThumbThreshold{ 0.25f }; // 25 cm

    const auto thumbDistance{ GetJointsDistance(joints, XR_HAND_JOINT_THUMB_METACARPAL_EXT, XR_HAND_JOINT_THUMB_TIP_EXT) };
    const auto thumbScore{ 1.0f - std::clamp(thumbDistance / curledThumbThreshold, 0.0f, 1.0f) };

    const float pokeActivationThreshold{ 0.4f };
    const auto pokeConfidence{ indexScore * averageCurlScore * thumbScore };
    const bool pokeActive{ pokeConfidence > pokeActivationThreshold };

    outPinchAction = {};
    outPinchAction.type = hand;
    outPinchAction.actionType = HandActionType::POKE;
    if (!pokeActive || !handValid) {
        return false;
    }

    const glm::vec3 pokeDirection(glm::normalize(GetJointPosition(joints, XR_HAND_JOINT_INDEX_TIP_EXT) - GetJointPosition(joints, XR_HAND_JOINT_INDEX_METACARPAL_EXT)));
    const glm::vec3 forwardVector(0.0f, 0.0f, -1.0f);

    const glm::quat pokeOrientation{ prev::util::math::CreateQuaternion(forwardVector, pokeDirection) };
    const glm::vec3 pokePosition{ GetJointPosition(joints, XR_HAND_JOINT_INDEX_TIP_EXT) };

    outPinchAction.pose = prev::util::math::Pose{ pokeOrientation, pokePosition };
    outPinchAction.active = true;
    outPinchAction.value = pokeConfidence;

    return true;
}
} // namespace prev::xr::input::util

#endif