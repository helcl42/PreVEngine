#include "WebXrInput.h"

#ifdef ENABLE_WEBXR

#include <emscripten.h>

#include "../../XrEvents.h"

#include "../../../event/EventChannel.h"

// clang-format off
EM_JS(int, prev_webxr_get_hand, (int handIndex, float* out), {
    const state = Module.__prevWebXr;
    if (!state || !state.frame || !state.session) {
        return 0;
    }
    // WebXR joint names in engine HandJointType order (index 0 = PALM has no WebXR joint).
    const JOINTS = [
        null, 'wrist',
        'thumb-metacarpal', 'thumb-phalanx-proximal', 'thumb-phalanx-distal', 'thumb-tip',
        'index-finger-metacarpal', 'index-finger-phalanx-proximal', 'index-finger-phalanx-intermediate', 'index-finger-phalanx-distal', 'index-finger-tip',
        'middle-finger-metacarpal', 'middle-finger-phalanx-proximal', 'middle-finger-phalanx-intermediate', 'middle-finger-phalanx-distal', 'middle-finger-tip',
        'ring-finger-metacarpal', 'ring-finger-phalanx-proximal', 'ring-finger-phalanx-intermediate', 'ring-finger-phalanx-distal', 'ring-finger-tip',
        'pinky-finger-metacarpal', 'pinky-finger-phalanx-proximal', 'pinky-finger-phalanx-intermediate', 'pinky-finger-phalanx-distal', 'pinky-finger-tip'
    ];
    const want = (handIndex === 0) ? 'left' : 'right';
    let src = null;
    for (const s of state.session.inputSources) {
        if (s.handedness === want && s.hand) {
            src = s;
            break;
        }
    }
    if (!src) {
        return 0;
    }
    const base = out >> 2;
    for (let j = 0; j < 26; ++j) {
        const o = base + j * 9;
        for (let k = 0; k < 9; ++k) {
            HEAPF32[o + k] = 0;
        }
        const name = JOINTS[j];
        if (!name) {
            continue;
        }
        const space = src.hand.get(name);
        if (!space) {
            continue;
        }
        const jp = state.frame.getJointPose(space, state.refSpace); // VERIFY
        if (!jp) {
            continue;
        }
        const p = jp.transform.position, q = jp.transform.orientation;
        HEAPF32[o + 0] = 1;
        HEAPF32[o + 1] = p.x;
        HEAPF32[o + 2] = p.y;
        HEAPF32[o + 3] = p.z;
        HEAPF32[o + 4] = q.x;
        HEAPF32[o + 5] = q.y;
        HEAPF32[o + 6] = q.z;
        HEAPF32[o + 7] = q.w;
        HEAPF32[o + 8] = jp.radius || 0;
    }
    return 1;
});

EM_JS(int, prev_webxr_get_controller, (int handIndex, float* out), {
    const state = Module.__prevWebXr;
    if (!state || !state.frame || !state.session) {
        return 0;
    }
    const want = (handIndex === 0) ? 'left' : 'right';
    let src = null;
    for (const s of state.session.inputSources) {
        if (s.handedness === want && s.gripSpace && s.gamepad) {
            src = s;
            break;
        }
    }
    if (!src) {
        return 0;
    }
    const base = out >> 2;
    for (let k = 0; k < 10; ++k) {
        HEAPF32[base + k] = 0;
    }
    const pose = state.frame.getPose(src.gripSpace, state.refSpace); // VERIFY
    if (pose) {
        const p = pose.transform.position, q = pose.transform.orientation;
        HEAPF32[base + 0] = p.x;
        HEAPF32[base + 1] = p.y;
        HEAPF32[base + 2] = p.z;
        HEAPF32[base + 3] = q.x;
        HEAPF32[base + 4] = q.y;
        HEAPF32[base + 5] = q.z;
        HEAPF32[base + 6] = q.w;
    }
    const gp = src.gamepad;
    const trigger = gp.buttons[0]; // standard xr-gamepad mapping: 0 = trigger, 1 = squeeze/grip
    const squeeze = gp.buttons[1];
    HEAPF32[base + 7] = squeeze ? squeeze.value : 0;
    HEAPF32[base + 8] = (trigger && trigger.pressed) ? 1 : 0;
    HEAPF32[base + 9] = (squeeze && squeeze.pressed) ? 1 : 0;
    return 1;
});
// clang-format on

namespace prev::xr::web_xr::input {
void WebXrInput::PollActions()
{
    HandleControllerActions();
    HandleHandTrackingActions();
}

void WebXrInput::HandleControllerActions()
{
    HandControllersEvent controllersEvent{};
    for (uint32_t h = 0; h < MAX_HAND_COUNT; ++h) {
        float c[10] = {}; // [0..2] position, [3..6] orientation xyzw, [7] squeeze, [8] trigger, [9] grip
        if (!prev_webxr_get_controller(static_cast<int>(h), c)) {
            continue;
        }
        auto& ctrl = controllersEvent.handControllers[h];
        ctrl.type = (h == 0) ? HandType::LEFT : HandType::RIGHT;
        ctrl.active = true;
        ctrl.pose = prev::util::math::Pose{ glm::quat{ c[6], c[3], c[4], c[5] }, glm::vec3{ c[0], c[1], c[2] } };
        ctrl.squeeze = c[7];
        ctrl.flags = {};
        ctrl.flags |= (c[8] != 0.0f) ? HandEventFlags::TRIGGER : HandEventFlags::NONE;
        ctrl.flags |= (c[9] != 0.0f) ? HandEventFlags::SQUEEZE : HandEventFlags::NONE;
    }
    prev::event::EventChannel::Post(controllersEvent);
}

void WebXrInput::HandleHandTrackingActions()
{
    HandsEvent handsEvent{};
    for (uint32_t h = 0; h < MAX_HAND_COUNT; ++h) {
        float data[MAX_HAND_TRACKING_JOINT_COUNT * 9] = {};
        if (!prev_webxr_get_hand(static_cast<int>(h), data)) {
            continue;
        }
        auto& hand = handsEvent.hands[h];
        hand.type = (h == 0) ? HandType::LEFT : HandType::RIGHT;
        hand.active = true;
        for (uint32_t j = 0; j < MAX_HAND_TRACKING_JOINT_COUNT; ++j) {
            const float* d = &data[j * 9];
            auto& joint = hand.joints[j];
            joint.type = static_cast<HandJointType>(j);
            joint.active = d[0] != 0.0f;
            joint.pose = prev::util::math::Pose{ glm::quat{ d[7], d[4], d[5], d[6] }, glm::vec3{ d[1], d[2], d[3] } };
            joint.radius = d[8];
        }
        hand.pose = hand.joints[1].pose; // wrist
    }
    prev::event::EventChannel::Post(handsEvent);
}
} // namespace prev::xr::web_xr::input

#endif
