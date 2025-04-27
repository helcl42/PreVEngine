#ifndef __OPEN_XR_INPUT_H__
#define __OPEN_XR_INPUT_H__

#ifdef ENABLE_XR

#include "../common/OpenXrCommon.h"
#include "../common/OpenXrContext.h"
#include "../common/IOpenXrEventObserver.h"

namespace prev::xr::input {
class OpenXrInput final : public common::IOpenXrEventObserver {
public:
    explicit OpenXrInput(common::OpenXrContext& context);

    ~OpenXrInput();

public:
    void Init();

    void ShutDown();

    void OnSessionCreate();

    void OnSessionDestroy();

public:
    void CreateActionSet();

    void DestroyActionSet();

    void CreateActionPoses();

    void DestroyActionPoses();

    void AttachActionSet();

    void DetachActionSet();

    bool SuggestBindings();

    void RecordCurrentBindings();

    void PollActions(const XrTime time);

public:
    void OnOpenXrEvent(const XrEventDataBuffer& evt) override;

private:
    common::OpenXrContext& m_context;

    XrActionSet m_actionSet{};

    XrAction m_squeezeAction{};
    XrActionStateFloat m_squeezeState[2] = { { XR_TYPE_ACTION_STATE_FLOAT }, { XR_TYPE_ACTION_STATE_FLOAT } };

    XrAction m_triggerAction{};
    XrActionStateBoolean m_triggerState[2] = { { XR_TYPE_ACTION_STATE_BOOLEAN }, { XR_TYPE_ACTION_STATE_BOOLEAN } };

    XrAction m_palmPoseAction{};
    XrPath m_handPaths[2] = { 0, 0 };
    XrSpace m_handPoseSpace[2];
    XrActionStatePose m_handPoseState[2] = { { XR_TYPE_ACTION_STATE_POSE }, { XR_TYPE_ACTION_STATE_POSE } };
    XrPosef m_handPose[2] = {
            { { 1.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
            { { 1.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }
    };
};
}

#endif

#endif