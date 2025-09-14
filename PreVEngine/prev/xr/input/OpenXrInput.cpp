#include "OpenXrInput.h"

#ifdef ENABLE_XR

#include "../XrEvents.h"
#include "../util/OpenXRUtil.h"

#include "../../event/EventChannel.h"
#include "../../util/MathUtils.h"

#include <vector>

namespace prev::xr::input {
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

    inline XrPath ConvertStringToXrPath(const XrInstance& instance, const char* pathString)
    {
        XrPath xrPath;
        OPENXR_CHECK(xrStringToPath(instance, pathString, &xrPath), "Failed to create XrPath from string.");
        return xrPath;
    }

    inline std::string ConvertXrPathToString(const XrInstance& instance, const XrPath& path)
    {
        uint32_t strLength;
        char text[XR_MAX_PATH_LENGTH];
        OPENXR_CHECK(xrPathToString(instance, path, XR_MAX_PATH_LENGTH, &strLength, text), "Failed to retrieve path.");
        return std::string(text);
    }

    inline XrAction CreateAction(const XrActionSet& actionSet, const char* name, const XrActionType xrActionType, const std::vector<XrPath>& subactionPaths)
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

    inline bool SuggestProfileBindings(const XrInstance& instance, const char* profilePath, const std::vector<XrActionSuggestedBinding>& bindings)
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

    inline XrSpace CreateActionSpace(const XrSession& session, const XrAction& xrAction, const XrPath& subactionPath)
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

    inline void DestroyActionSpace(XrSpace& space)
    {
        OPENXR_CHECK(xrDestroySpace(space), "Failed to destroy ActionSpace.")
        space = {};
    }

    inline prev::util::math::Pose ConvertXrPoseToPose(const XrPosef& xrPose)
    {
        return prev::util::math::Pose{
            { xrPose.orientation.w, xrPose.orientation.x, xrPose.orientation.y, xrPose.orientation.z },
            { xrPose.position.x, xrPose.position.y, xrPose.position.z },
        };
    }

    inline XrPosef ConvertPoseToXrPose(const prev::util::math::Pose& pose)
    {
        return XrPosef{
            { pose.orientation.x, pose.orientation.y, pose.orientation.z, pose.orientation.w },
            { pose.position.x, pose.position.y, pose.position.z }
        };
    }

    inline std::optional<prev::util::math::Pose> GetPoseState(const XrSession& session, const XrTime& time, const XrAction& action, const XrPath& subActionPath, const XrSpace& baseSpace, const XrSpace& space)
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

    inline std::optional<float> GetFloatState(const XrSession& session, const XrAction& action, const XrPath& subActionPath)
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

    inline std::optional<bool> GetBoolState(const XrSession& session, const XrAction& action, const bool invert, const XrPath& subActionPath)
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

    template <typename IndexType>
    inline IndexType ConvertHandTypeToIndex(const HandType type)
    {
        return static_cast<IndexType>(type);
    }

    template <typename IndexType>
    inline HandType ConvertIndexToHandType(const IndexType index)
    {
        return static_cast<HandType>(index);
    }

    inline glm::vec3 GetJointPosition(const XrHandJointLocationsEXT& joints, const XrHandJointEXT jointId)
    {
        const auto& joint{ joints.jointLocations[jointId] };
        const auto jointPosition{ glm::vec3{ joint.pose.position.x, joint.pose.position.y, joint.pose.position.z } };
        return jointPosition;
    }

    inline float GetJointsDistance(const XrHandJointLocationsEXT& joints, const XrHandJointEXT jointId1, const XrHandJointEXT jointId2)
    {
        const auto joint1Position{ GetJointPosition(joints, jointId1) };
        const auto joint2Position{ GetJointPosition(joints, jointId2) };
        const auto jointDistance{ glm::distance(joint1Position, joint2Position) };
        return jointDistance;
    }

    inline bool DetectPinch(const HandType hand, const XrHandJointLocationsEXT& joints, XrHandActionEvent& outPinchAction)
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

    inline bool DetectAim(const HandType hand, const XrHandJointLocationsEXT& joints, XrHandActionEvent& outPinchAction)
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

    inline bool DetectPoke(const HandType hand, const XrHandJointLocationsEXT& joints, XrHandActionEvent& outPinchAction)
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

    XrActionsSyncInfo actionsSyncInfo{ prev::xr::util::CreateStruct<XrActionsSyncInfo>(XR_TYPE_ACTIONS_SYNC_INFO) };
    actionsSyncInfo.countActiveActionSets = 1;
    actionsSyncInfo.activeActionSets = &activeActionSet;
    OPENXR_CHECK(xrSyncActions(m_session, &actionsSyncInfo), "Failed to sync Actions.");

    HandleControllerActions(time);
    HandleHandTrackingActions(time);
}

void OpenXrInput::operator()(const XrHapticFeedback& hapticFeedback)
{
    const auto handIndex{ ConvertHandTypeToIndex<uint32_t>(hapticFeedback.type) };
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
    XrActionSetCreateInfo actionSetCreateInfo{ prev::xr::util::CreateStruct<XrActionSetCreateInfo>(XR_TYPE_ACTION_SET_CREATE_INFO) };
    actionSetCreateInfo.priority = 0;
    strncpy(actionSetCreateInfo.actionSetName, "prev-engine-action-set", XR_MAX_ACTION_SET_NAME_SIZE);
    strncpy(actionSetCreateInfo.localizedActionSetName, "prev-engine-action-set", XR_MAX_LOCALIZED_ACTION_SET_NAME_SIZE);
    OPENXR_CHECK(xrCreateActionSet(m_instance, &actionSetCreateInfo, &m_actionSet), "Failed to create ActionSet.");

    for (size_t i = 0; i < m_handPathStrings.size(); ++i) {
        m_handPaths[i] = ConvertStringToXrPath(m_instance, m_handPathStrings[i]);
    }

    // Controllers
    m_squeezeAction = CreateAction(m_actionSet, "squeeze", XR_ACTION_TYPE_FLOAT_INPUT, { m_handPaths.begin(), m_handPaths.end() });
    m_triggerAction = CreateAction(m_actionSet, "trigger", XR_ACTION_TYPE_BOOLEAN_INPUT, { m_handPaths.begin(), m_handPaths.end() });
    m_palmPoseAction = CreateAction(m_actionSet, "pose", XR_ACTION_TYPE_POSE_INPUT, { m_handPaths.begin(), m_handPaths.end() });
    m_quitAction = CreateAction(m_actionSet, "quit", XR_ACTION_TYPE_BOOLEAN_INPUT, { m_handPaths.begin(), m_handPaths.end() });
    m_vibrateAction = CreateAction(m_actionSet, "vibrate", XR_ACTION_TYPE_VIBRATION_OUTPUT, { m_handPaths.begin(), m_handPaths.end() });

    // HandsTracking
    m_poseAction = CreateAction(m_actionSet, "hand_pose", XR_ACTION_TYPE_POSE_INPUT, { m_handPaths.begin(), m_handPaths.end() });
    m_grabAction = CreateAction(m_actionSet, "hand_grab", XR_ACTION_TYPE_FLOAT_INPUT, { m_handPaths.begin(), m_handPaths.end() });
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
    anyOk |= SuggestProfileBindings(m_instance, "/interaction_profiles/khr/simple_controller", {
        { m_triggerAction, ConvertStringToXrPath(m_instance, "/user/hand/left/input/select/click") },
        { m_squeezeAction, ConvertStringToXrPath(m_instance, "/user/hand/right/input/select/click") },
        { m_palmPoseAction, ConvertStringToXrPath(m_instance, "/user/hand/left/input/grip/pose") },
        { m_palmPoseAction, ConvertStringToXrPath(m_instance, "/user/hand/right/input/grip/pose") },
        { m_quitAction, ConvertStringToXrPath(m_instance, "/user/hand/left/input/menu/click") },
        { m_quitAction, ConvertStringToXrPath(m_instance, "/user/hand/right/input/menu/click") },
        { m_vibrateAction, ConvertStringToXrPath(m_instance, "/user/hand/left/output/haptic") },
        { m_vibrateAction, ConvertStringToXrPath(m_instance, "/user/hand/right/output/haptic") },
    });
    // clang-format on
    // clang-format off
    anyOk |= SuggestProfileBindings(m_instance, "/interaction_profiles/oculus/touch_controller", { 
        { m_squeezeAction, ConvertStringToXrPath(m_instance, "/user/hand/left/input/squeeze/value") }, 
        { m_squeezeAction, ConvertStringToXrPath(m_instance, "/user/hand/right/input/squeeze/value") }, 
        { m_triggerAction, ConvertStringToXrPath(m_instance, "/user/hand/left/input/trigger/value") }, 
        { m_triggerAction, ConvertStringToXrPath(m_instance, "/user/hand/right/input/trigger/value") }, 
        { m_palmPoseAction, ConvertStringToXrPath(m_instance, "/user/hand/left/input/grip/pose") }, 
        { m_palmPoseAction, ConvertStringToXrPath(m_instance, "/user/hand/right/input/grip/pose") },
        { m_quitAction, ConvertStringToXrPath(m_instance, "/user/hand/left/input/menu/click") },
        { m_vibrateAction, ConvertStringToXrPath(m_instance, "/user/hand/left/output/haptic") },
        { m_vibrateAction, ConvertStringToXrPath(m_instance, "/user/hand/right/output/haptic") },
    });
    // clang-format on
    // clang-format off
    anyOk |= SuggestProfileBindings(m_instance, "/interaction_profiles/htc/vive_controller", { 
        { m_squeezeAction, ConvertStringToXrPath(m_instance, "/user/hand/left/input/trigger/value") }, 
        { m_squeezeAction, ConvertStringToXrPath(m_instance, "/user/hand/right/input/trigger/value") }, 
        { m_palmPoseAction, ConvertStringToXrPath(m_instance, "/user/hand/left/input/grip/pose") }, 
        { m_palmPoseAction, ConvertStringToXrPath(m_instance, "/user/hand/right/input/grip/pose") },
        { m_quitAction, ConvertStringToXrPath(m_instance, "/user/hand/left/input/menu/click") },
        { m_quitAction, ConvertStringToXrPath(m_instance, "/user/hand/right/input/menu/click") },
        { m_vibrateAction, ConvertStringToXrPath(m_instance, "/user/hand/left/output/haptic") },
        { m_vibrateAction, ConvertStringToXrPath(m_instance, "/user/hand/right/output/haptic") },
    });
    // clang-format on
    // clang-format off
    anyOk |= SuggestProfileBindings(m_instance, "/interaction_profiles/valve/index_controller", { 
        { m_squeezeAction, ConvertStringToXrPath(m_instance, "/user/hand/left/input/squeeze/value") }, 
        { m_squeezeAction, ConvertStringToXrPath(m_instance, "/user/hand/right/input/squeeze/value") },
        { m_palmPoseAction, ConvertStringToXrPath(m_instance, "/user/hand/left/input/grip/pose") }, 
        { m_palmPoseAction, ConvertStringToXrPath(m_instance, "/user/hand/right/input/grip/pose") },
        { m_quitAction, ConvertStringToXrPath(m_instance, "/user/hand/left/input/b/click") },
        { m_quitAction, ConvertStringToXrPath(m_instance, "/user/hand/right/input/b/click") },
        { m_vibrateAction, ConvertStringToXrPath(m_instance, "/user/hand/left/output/haptic") },
        { m_vibrateAction, ConvertStringToXrPath(m_instance, "/user/hand/right/output/haptic") },
    });
    // clang-format on
    // clang-format off
    anyOk |= SuggestProfileBindings(m_instance, "/interaction_profiles/microsoft/motion_controller", {
        { m_squeezeAction, ConvertStringToXrPath(m_instance, "/user/hand/left/input/trigger/value") },
        { m_squeezeAction, ConvertStringToXrPath(m_instance, "/user/hand/right/input/trigger/value") },
        { m_palmPoseAction, ConvertStringToXrPath(m_instance, "/user/hand/left/input/grip/pose") },
        { m_palmPoseAction, ConvertStringToXrPath(m_instance, "/user/hand/right/input/grip/pose") },
        { m_quitAction, ConvertStringToXrPath(m_instance, "/user/hand/left/input/menu/click") },
        { m_quitAction, ConvertStringToXrPath(m_instance, "/user/hand/right/input/menu/click") },
        { m_vibrateAction, ConvertStringToXrPath(m_instance, "/user/hand/left/output/haptic") },
        { m_vibrateAction, ConvertStringToXrPath(m_instance, "/user/hand/right/output/haptic") },
    });
    // clang-format on
    return anyOk;
}

void OpenXrInput::CreateActionSpaces()
{
    for (size_t i = 0; i < m_handPaths.size(); ++i) {
        m_handPoseSpace[i] = CreateActionSpace(m_session, m_palmPoseAction, m_handPaths[i]);
    }

    for (size_t i = 0; i < m_handPaths.size(); ++i) {
        m_handSpace[i] = CreateActionSpace(m_session, m_poseAction, m_handPaths[i]);
    }
}

void OpenXrInput::DestroyActionSpaces()
{
    for (size_t i = 0; i < m_handPaths.size(); ++i) {
        DestroyActionSpace(m_handSpace[i]);
    }

    for (size_t i = 0; i < m_handPaths.size(); ++i) {
        DestroyActionSpace(m_handPoseSpace[i]);
    }
}

void OpenXrInput::AttachActionSet()
{
    // Attach the action set we just made to the session. We could attach multiple action sets!
    XrSessionActionSetsAttachInfo actionSetAttachInfo{ prev::xr::util::CreateStruct<XrSessionActionSetsAttachInfo>(XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO) };
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
        XrHandTrackerCreateInfoEXT xrHandTrackerCreateInfo{ prev::xr::util::CreateStruct<XrHandTrackerCreateInfoEXT>(XR_TYPE_HAND_TRACKER_CREATE_INFO_EXT) };
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
        XrInteractionProfileState interactionProfile{ prev::xr::util::CreateStruct<XrInteractionProfileState>(XR_TYPE_INTERACTION_PROFILE_STATE) };
        OPENXR_CHECK(xrGetCurrentInteractionProfile(m_session, m_handPaths[i], &interactionProfile), "Failed to get profile.");
        if (interactionProfile.interactionProfile) {
            LOGI("%s ActiveProfile: %s", m_handPathStrings[i], ConvertXrPathToString(m_instance, interactionProfile.interactionProfile).c_str());
        }
    }
}

void OpenXrInput::HandleControllerActions(const XrTime time)
{
    // input events
    {
        XrHandControllersEvent handControllersEvent{};

        for (size_t i = 0; i < m_handPaths.size(); ++i) {
            const auto handPose{ GetPoseState(m_session, time, m_palmPoseAction, m_handPaths[i], m_localSpace, m_handPoseSpace[i]) };
            const auto squeeze{ GetFloatState(m_session, m_squeezeAction, m_handPaths[i]) };
            const auto trigger{ GetFloatState(m_session, m_squeezeAction, m_handPaths[i]) };

            auto& handControllerEvent{ handControllersEvent.handControllers[i] };
            handControllerEvent.type = ConvertIndexToHandType(i);
            handControllerEvent.active = handPose.has_value();
            handControllerEvent.pose = handPose ? *handPose : prev::util::math::Pose{};
            handControllerEvent.flags = {};
            handControllerEvent.flags |= squeeze ? HandEventFlags::SQUEEZE : HandEventFlags::NONE;
            handControllerEvent.flags |= trigger ? HandEventFlags::TRIGGER : HandEventFlags::NONE;
            handControllerEvent.squeeze = squeeze ? *squeeze : 1.0f;
        }

        prev::event::EventChannel::Post(handControllersEvent);
    }

    {
        const auto quit{ GetBoolState(m_session, m_quitAction, true, XR_NULL_PATH) };
        if (quit) {
            // TODO - should we just generate quit event or request exit session direcly?
            xrRequestExitSession(m_session);
        }
    }

    // output events
    for (size_t i = 0; i < m_handPaths.size(); ++i) {
        const auto& hapticFeedbackEvent{ m_hapticFeedbackEvents[i] };
        if (hapticFeedbackEvent) {
            XrHapticVibration hapticFeedback{ prev::xr::util::CreateStruct<XrHapticVibration>(XR_TYPE_HAPTIC_VIBRATION) };
            hapticFeedback.amplitude = hapticFeedbackEvent->amplitude;
            hapticFeedback.duration = hapticFeedbackEvent->duration;

            XrHapticActionInfo hapticAction{ prev::xr::util::CreateStruct<XrHapticActionInfo>(XR_TYPE_HAPTIC_ACTION_INFO) };
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

    {
        XrHandsEvent handsEvent{};

        for (size_t i = 0; i < m_handPaths.size(); ++i) {
            const auto& hand{ m_hands[i] };

            const auto handPose{ GetPoseState(m_session, time, m_poseAction, m_handPaths[i], m_localSpace, m_handSpace[i]) };
            const auto squeeze{ GetFloatState(m_session, m_grabAction, m_handPaths[i]) };

            XrHandJointsMotionRangeInfoEXT motionRangeInfo{ prev::xr::util::CreateStruct<XrHandJointsMotionRangeInfoEXT>(XR_TYPE_HAND_JOINTS_MOTION_RANGE_INFO_EXT) };
            motionRangeInfo.handJointsMotionRange = XR_HAND_JOINTS_MOTION_RANGE_UNOBSTRUCTED_EXT; // XR_HAND_JOINTS_MOTION_RANGE_CONFORMING_TO_CONTROLLER_EXT

            XrHandJointsLocateInfoEXT locateInfo{ prev::xr::util::CreateStruct<XrHandJointsLocateInfoEXT>(XR_TYPE_HAND_JOINTS_LOCATE_INFO_EXT) };
            locateInfo.next = &motionRangeInfo;
            locateInfo.baseSpace = m_localSpace;
            locateInfo.time = time;

            XrHandJointLocationEXT jointLocations[XR_HAND_JOINT_COUNT_EXT]{};
            XrHandJointLocationsEXT locations{ prev::xr::util::CreateStruct<XrHandJointLocationsEXT>(XR_TYPE_HAND_JOINT_LOCATIONS_EXT) };
            locations.jointCount = XR_HAND_JOINT_COUNT_EXT;
            locations.jointLocations = jointLocations;
            OPENXR_CHECK(xrLocateHandJointsEXT(hand, &locateInfo, &locations), "Failed to locate hand joints.");

            auto& handEvent{ handsEvent.hands[i] };
            handEvent.type = ConvertIndexToHandType(i);
            handEvent.active = locations.isActive;
            handEvent.pose = handPose ? *handPose : prev::util::math::Pose{};
            handEvent.squeeze = squeeze ? *squeeze : 1.0f;
            for (uint32_t j = 0; j < locations.jointCount; ++j) {
                const auto& jointLocation{ locations.jointLocations[j] };
                auto& handJoint{ handEvent.joints[j] };
                handJoint.active = (jointLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) != 0 && (jointLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) != 0;
                handJoint.pose = ConvertXrPoseToPose(jointLocation.pose);
                handJoint.radius = jointLocation.radius;
            }
        }

        prev::event::EventChannel::Post(handsEvent);
    }
    {
        XrHandsActionsEvent handsActionsEvent{};

        for (size_t i = 0; i < m_handPaths.size(); ++i) {
            const auto& hand{ m_hands[i] };

            XrHandJointsMotionRangeInfoEXT motionRangeInfo{ prev::xr::util::CreateStruct<XrHandJointsMotionRangeInfoEXT>(XR_TYPE_HAND_JOINTS_MOTION_RANGE_INFO_EXT) };
            motionRangeInfo.handJointsMotionRange = XR_HAND_JOINTS_MOTION_RANGE_UNOBSTRUCTED_EXT;

            XrHandJointsLocateInfoEXT locateInfo{ prev::xr::util::CreateStruct<XrHandJointsLocateInfoEXT>(XR_TYPE_HAND_JOINTS_LOCATE_INFO_EXT) };
            locateInfo.next = &motionRangeInfo;
            locateInfo.baseSpace = m_localSpace;
            locateInfo.time = time;

            XrHandJointLocationEXT jointLocations[XR_HAND_JOINT_COUNT_EXT]{};
            XrHandJointLocationsEXT locations{ prev::xr::util::CreateStruct<XrHandJointLocationsEXT>(XR_TYPE_HAND_JOINT_LOCATIONS_EXT) };
            locations.jointCount = XR_HAND_JOINT_COUNT_EXT;
            locations.jointLocations = jointLocations;
            OPENXR_CHECK(xrLocateHandJointsEXT(hand, &locateInfo, &locations), "Failed to locate hand joints.");

            auto& handsActions{ handsActionsEvent.handsActions[i] };

            uint32_t actionEventIndex{ 0 };
            {
                auto& actionEvent{ handsActions.actions[actionEventIndex] };
                if (DetectPinch(ConvertIndexToHandType(i), locations, actionEvent)) {
                    LOGI("Pinch detected! Value: %f, Position = (%f, %f, %f).", actionEvent.value, actionEvent.pose.position.x, actionEvent.pose.position.y, actionEvent.pose.position.z);
                }
                handsActions.actionCount = ++actionEventIndex;
            }
            {
                auto& actionEvent{ handsActions.actions[actionEventIndex] };
                if (DetectAim(ConvertIndexToHandType(i), locations, actionEvent)) {
                    LOGI("Aim detected! Value: %f, Position = (%f, %f, %f).", actionEvent.value, actionEvent.pose.position.x, actionEvent.pose.position.y, actionEvent.pose.position.z);
                }
                handsActions.actionCount = ++actionEventIndex;
            }
            {
                auto& actionEvent{ handsActions.actions[actionEventIndex] };
                if (DetectPoke(ConvertIndexToHandType(i), locations, actionEvent)) {
                    LOGI("Poke detected! Value: %f, Position = (%f, %f, %f).", actionEvent.value, actionEvent.pose.position.x, actionEvent.pose.position.y, actionEvent.pose.position.z);
                }
                handsActions.actionCount = ++actionEventIndex;
            }
        }

        prev::event::EventChannel::Post(handsActionsEvent);
    }
}
} // namespace prev::xr::input

#endif
