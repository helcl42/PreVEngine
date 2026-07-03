#include "WebXr.h"

#ifdef ENABLE_WEBXR

#include <emscripten.h>
#include <webgpu/webgpu.h>

#include "../XrEvents.h"

#include "../../common/Logger.h"
#include "../../event/EventChannel.h"

#include <cmath>
#include <functional>

// clang-format off
EM_JS(int, prev_webxr_is_supported, (), {
    return (navigator.xr && navigator.xr.isSessionSupported) ? 1 : 0;
});

EM_JS(int, prev_webxr_preferred_color_format, (), {
    if (!navigator.gpu || !navigator.gpu.getPreferredCanvasFormat) { return 0; }
    const f = navigator.gpu.getPreferredCanvasFormat();
    if (f === 'rgba8unorm') { return 1; }
    if (f === 'bgra8unorm') { return 2; }
    return 0;
});

EM_JS(void, prev_webxr_install_enter_button, (void* self, void* devicePtr), {
    let btn = document.getElementById('prev-enter-vr');
    if (!btn) {
        btn = document.createElement('button');
        btn.id = 'prev-enter-vr';
        btn.textContent = 'Enter VR';
        btn.style.cssText = 'position:fixed;left:50%;top:50%;transform:translate(-50%,-50%);' +
                            'padding:16px 28px;font-size:20px;z-index:9999;cursor:pointer;';
        document.body.appendChild(btn);
    }
    btn.onclick = async function() {
        try {
            if (!navigator.xr || !(await navigator.xr.isSessionSupported('immersive-vr'))) {
                console.error('WebXR: immersive-vr not supported');
                return;
            }
            const session = await navigator.xr.requestSession('immersive-vr', { requiredFeatures: ['webgpu'], optionalFeatures: ['hand-tracking', 'local-floor'] });
            let refSpace;
            try { refSpace = await session.requestReferenceSpace('local-floor'); }
            catch (e) { refSpace = await session.requestReferenceSpace('local'); }

            const state = { session: session, refSpace: refSpace, frame: null, running: true, visible: true,
                            binding: null, layer: null,
                            colorTexturePtr: 0, devicePtr: devicePtr, extentW: 0, extentH: 0, deltaTime: 0.0 };
            Module.__prevWebXr = state;
            session.addEventListener('end', function() {
                if (Module.__prevWebXr === state) { state.running = false; }
                btn.style.display = 'block'; // session ended - re-show the button so the user can re-enter VR
            });
            session.addEventListener('visibilitychange', function() {
                state.visible = (session.visibilityState === 'visible'); // hidden/blurred when false
            });

            const device = WebGPU.Internals.jsObjects[devicePtr];
            const fmt = navigator.gpu.getPreferredCanvasFormat();
            const binding = new XRGPUBinding(session, device); // VERIFY: experimental API
            const layer = binding.createProjectionLayer({ colorFormat: fmt, textureType: 'texture-array' });
            session.updateRenderState({ layers: [layer] });
            state.binding = binding;
            state.layer = layer;
            state.colorFormat = fmt;
            state.extentW = layer.textureWidth | 0;   // undefined -> 0
            state.extentH = layer.textureHeight | 0;
            btn.style.display = 'none';

            const onXrFrame = function(time, frame) {
                if (!state.running) { return; }
                // Keep the rAF alive but skip rendering while the session is hidden/blurred.
                if (state.visible === false) { session.requestAnimationFrame(onXrFrame); return; }
                if (state.prevTime !== undefined) { state.deltaTime = (time - state.prevTime) / 1000.0; }
                state.prevTime = time;
                state.frame = frame;
                _prev_webxr_dispatch_frame(self);
                session.requestAnimationFrame(onXrFrame);
            };
            session.requestAnimationFrame(onXrFrame);
        } catch (e) {
            console.error('WebXR: failed to start immersive session:', e);
        }
    };
});

EM_JS(int, prev_webxr_acquire_views, (), {
    const state = Module.__prevWebXr;
    if (!state || !state.frame || !state.binding || !state.layer) {
        return 0;
    }
    const pose = state.frame.getViewerPose(state.refSpace);
    if (!pose || pose.views.length === 0) {
        return 0;
    }
    state.pose = pose;
    const sub = state.binding.getViewSubImage(state.layer, pose.views[0]);
    state.colorTexturePtr = WebGPU.importJsTexture(sub.colorTexture, state.devicePtr || 0);
    state.extentW = sub.viewport.width;
    state.extentH = sub.viewport.height;
    return pose.views.length;
});

EM_JS(int, prev_webxr_view_width, (), {
    const state = Module.__prevWebXr;
    return state ? (state.extentW | 0) : 0;
});

EM_JS(int, prev_webxr_view_height, (), {
    const state = Module.__prevWebXr;
    return state ? (state.extentH | 0) : 0;
});

EM_JS(void*, prev_webxr_color_texture, (), {
    const state = Module.__prevWebXr;
    return state ? (state.colorTexturePtr || 0) : 0;
});

// Write view i's transform (pos xyz + quat xyzw) and 16-float projection matrix into out[0..22].
EM_JS(int, prev_webxr_get_view_data, (int i, float* out), {
    const state = Module.__prevWebXr;
    if (!state || !state.pose || i >= state.pose.views.length) {
        return 0;
    }
    const v = state.pose.views[i];
    const p = v.transform.position;
    const o = v.transform.orientation;
    const m = v.projectionMatrix;
    const base = out >> 2;
    HEAPF32[base + 0] = p.x;
    HEAPF32[base + 1] = p.y;
    HEAPF32[base + 2] = p.z;
    HEAPF32[base + 3] = o.x;
    HEAPF32[base + 4] = o.y;
    HEAPF32[base + 5] = o.z;
    HEAPF32[base + 6] = o.w;
    for (let k = 0; k < 16; ++k) {
        HEAPF32[base + 7 + k] = m[k];
    }
    return 1;
});

EM_JS(void, prev_webxr_end_session, (), {
    const state = Module.__prevWebXr;
    if (state && state.session) {
        state.running = false;
        state.session.end();
    }
    Module.__prevWebXr = null;
});

EM_JS(void, prev_webxr_stop_frame_loop, (), {
    if (Module.__prevWebXr) {
        Module.__prevWebXr.running = false;
    }
});

EM_JS(int, prev_webxr_is_running, (), {
    const state = Module.__prevWebXr;
    return (state && state.running) ? 1 : 0;
});

EM_JS(double, prev_webxr_delta_time, (), {
    const state = Module.__prevWebXr;
    return state ? (state.deltaTime || 0.0) : 0.0;
});

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

namespace {
prev::util::math::Fov FovFromProjection(const float* m)
{
    prev::util::math::Fov fov{};
    fov.angleRight = std::atan((1.0f + m[8]) / m[0]);
    fov.angleLeft = std::atan((m[8] - 1.0f) / m[0]);
    fov.angleUp = std::atan((1.0f + m[9]) / m[5]);
    fov.angleDown = std::atan((m[9] - 1.0f) / m[5]);
    return fov;
}
} // namespace

extern "C" EMSCRIPTEN_KEEPALIVE void prev_webxr_dispatch_frame(void* self)
{
    if (self) {
        static_cast<prev::xr::web_xr::WebXr*>(self)->DispatchFrame();
    }
}

namespace prev::xr::web_xr {
WebXr::WebXr()
{
    if (!prev_webxr_is_supported()) {
        LOGW("WebXr: navigator.xr unavailable - WebXR is not supported in this browser");
    }

    switch (prev_webxr_preferred_color_format()) {
    case 1:
        m_colorFormat = GFX_FORMAT_R8G8B8A8_UNORM;
        break;
    case 2:
        m_colorFormat = GFX_FORMAT_B8G8R8A8_UNORM;
        break;
    default:
        break;
    }
    LOGI("WebXr: preferred color format = %s", (m_colorFormat == GFX_FORMAT_R8G8B8A8_UNORM) ? "rgba8unorm" : "bgra8unorm");
}

WebXr::~WebXr()
{
    ReleaseFrameTextures();
}

std::vector<std::string> WebXr::GetRequiredInstanceExtensions() const
{
    return {};
}

std::vector<std::string> WebXr::GetRequiredDeviceExtensions() const
{
    return {};
}

GfxAdapter WebXr::GetAdapter(GfxInstance /*instance*/) const
{
    return {};
}

void WebXr::UpdateGraphicsBinding(GfxInstance /*instance*/, GfxAdapter /*adapter*/, GfxDevice device, GfxQueue /*queue*/)
{
    m_device = device;
}

void WebXr::CreateSession()
{
    void* nativeDevice = nullptr;
    if (m_device) {
        gfxDeviceGetNativeHandle(m_device, &nativeDevice);
    }
    prev_webxr_install_enter_button(this, nativeDevice);
    LOGI("WebXr: 'Enter VR' button installed - click it to start the immersive session");
}

void WebXr::DestroySession()
{
    ReleaseFrameTextures();
    prev_webxr_end_session();
}

void WebXr::PollEvents()
{
}

void WebXr::PollActions()
{
    // Hands
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

    // Controllers
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

bool WebXr::BeginFrame()
{
    if (!prev_webxr_is_running()) {
        return false;
    }
    AcquireFrameTextures();
    PostCameraEvent();
    return !m_colorImages.empty();
}

bool WebXr::EndFrame()
{
    return prev_webxr_is_running() != 0;
}

void WebXr::AcquireFrameTextures()
{
    ReleaseFrameTextures();

    const int viewCount = prev_webxr_acquire_views();
    if (viewCount <= 0) {
        return;
    }
    m_viewCount = static_cast<uint32_t>(viewCount);
    m_extent = { static_cast<uint32_t>(prev_webxr_view_width()), static_cast<uint32_t>(prev_webxr_view_height()) };

    void* wgpuTexture = prev_webxr_color_texture();
    if (!wgpuTexture) {
        return;
    }

    GfxTextureImportDescriptor desc{};
    desc.sType = GFX_STRUCTURE_TYPE_TEXTURE_IMPORT_DESCRIPTOR;
    desc.label = "WebXRColor";
    desc.nativeHandle = wgpuTexture;
    desc.type = GFX_TEXTURE_TYPE_2D;
    desc.size = { m_extent.width, m_extent.height, 1 };
    desc.arrayLayerCount = m_viewCount;
    desc.mipLevelCount = 1;
    desc.sampleCount = GFX_SAMPLE_COUNT_1;
    desc.format = m_colorFormat;
    desc.usage = GFX_TEXTURE_USAGE_RENDER_ATTACHMENT;
    desc.currentLayout = GFX_TEXTURE_LAYOUT_COLOR_ATTACHMENT;

    GfxTexture texture{};
    if (gfxDeviceImportTexture(m_device, &desc, &texture) == GFX_RESULT_SUCCESS) {
        m_colorImages.push_back(texture);
    }
}

void WebXr::ReleaseFrameTextures()
{
    for (GfxTexture texture : m_colorImages) {
        if (!texture) {
            continue;
        }

        void* nativeHandle = nullptr;
        gfxTextureGetNativeHandle(texture, &nativeHandle);
        gfxTextureDestroy(texture);
        if (nativeHandle) {
            wgpuTextureRelease(static_cast<WGPUTexture>(nativeHandle));
        }
    }
    m_colorImages.clear();
}

void WebXr::PostCameraEvent()
{
    CameraEvent event{};
    const uint32_t maxEyes = static_cast<uint32_t>(MAX_VIEW_COUNT_VALUE);
    const uint32_t viewCount = (m_viewCount < maxEyes) ? m_viewCount : maxEyes;
    for (uint32_t i = 0; i < viewCount; ++i) {
        float data[23] = {}; // [0..2] position, [3..6] orientation xyzw, [7..22] projection (column-major)
        if (!prev_webxr_get_view_data(static_cast<int>(i), data)) {
            continue;
        }
        event.poses[i] = prev::util::math::Pose{ glm::quat{ data[6], data[3], data[4], data[5] }, glm::vec3{ data[0], data[1], data[2] } };
        event.fovs[i] = FovFromProjection(&data[7]);
    }
    event.count = viewCount;
    prev::event::EventChannel::Post(event);
}

bool WebXr::GetFrameImages(XrFrameImages& outImages) const
{
    if (m_colorImages.empty()) {
        return false; // no XR frame acquired yet
    }
    outImages.colorImage = m_colorImages[0];
    outImages.depthImage = {};
    outImages.imageIndex = 0;
    outImages.imagesChanged = true;
    return true;
}

uint32_t WebXr::GetImageCount() const
{
    return 1;
}

bool WebXr::HasDepthImages() const
{
    return false;
}

GfxExtent2D WebXr::GetExtent() const
{
    // Live size from JS: seeded from the projection layer at session start (valid before the first frame)
    // and refreshed each frame from the sub-image viewport. Avoids a transient 0x0 that would size
    // dependent targets (e.g. water reflection/refraction) to zero.
    return { static_cast<uint32_t>(prev_webxr_view_width()), static_cast<uint32_t>(prev_webxr_view_height()) };
}

GfxFormat WebXr::GetColorFormat() const
{
    return m_colorFormat;
}

GfxFormat WebXr::GetDepthFormat() const
{
    return GFX_FORMAT_DEPTH32_FLOAT;
}

uint32_t WebXr::GetViewCount() const
{
    return m_viewCount;
}

float WebXr::GetCurrentDeltaTime() const
{
    return static_cast<float>(prev_webxr_delta_time());
}

void WebXr::RunFrameLoop(const std::function<bool()>& tick)
{
    // Arm only; the rAF loop starts from the 'Enter VR' click (immersive sessions need a user gesture).
    m_frameCallback = tick;
}

void WebXr::DispatchFrame()
{
    // Do NOT reassign m_frameCallback from in here - it is the callable currently executing.
    if (m_frameCallback && !m_frameCallback()) {
        prev_webxr_stop_frame_loop();
    }
}
} // namespace prev::xr::web_xr

#endif
