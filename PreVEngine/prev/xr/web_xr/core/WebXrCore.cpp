#include "WebXrCore.h"

#ifdef ENABLE_WEBXR

#include <emscripten.h>

#include "../../../common/Logger.h"

// clang-format off
EM_JS(int, prev_webxr_is_supported, (), {
    return (navigator.xr && navigator.xr.isSessionSupported) ? 1 : 0;
});

EM_JS(void, prev_webxr_install_enter_buttons, (void* self, void* devicePtr), {
    const buttons = [];
    const showButtons = function() {
        if (Module.__prevWebXr && Module.__prevWebXr.running) { return; }
        buttons.forEach(function(b) { b.btn.style.display = b.supported ? 'block' : 'none'; });
    };
    const hideButtons = function() {
        buttons.forEach(function(b) { b.btn.style.display = 'none'; });
    };

    const startSession = async function(mode) {
        try {
            const session = await navigator.xr.requestSession(mode, { requiredFeatures: ['webgpu'], optionalFeatures: ['hand-tracking', 'local-floor'] });
            let refSpace;
            try { refSpace = await session.requestReferenceSpace('local-floor'); }
            catch (e) { refSpace = await session.requestReferenceSpace('local'); }

            const state = { session: session, refSpace: refSpace, frame: null, running: true, visible: true,
                            binding: null, layer: null, blendMode: session.environmentBlendMode,
                            colorTexturePtr: 0, devicePtr: devicePtr, extentW: 0, extentH: 0, deltaTime: 0.0 };
            Module.__prevWebXr = state;
            session.addEventListener('end', function() {
                if (Module.__prevWebXr === state) { state.running = false; }
                showButtons(); // allow re-entering (either mode)
            });
            session.addEventListener('visibilitychange', function() {
                state.visible = (session.visibilityState === 'visible');
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
            hideButtons();

            const onXrFrame = function(time, frame) {
                if (!state.running) { return; }
                if (state.visible === false) { session.requestAnimationFrame(onXrFrame); return; } // keep rAF alive while hidden
                if (state.prevTime !== undefined) { state.deltaTime = (time - state.prevTime) / 1000.0; }
                state.prevTime = time;
                state.frame = frame;
                _prev_webxr_dispatch_frame(self);
                session.requestAnimationFrame(onXrFrame);
            };
            session.requestAnimationFrame(onXrFrame);
        } catch (e) {
            console.error('WebXR: failed to start ' + mode + ' session:', e);
        }
    };

    const MODES = [
        { id: 'prev-enter-vr', label: 'Enter VR', mode: 'immersive-vr', dx: '-105%' },
        { id: 'prev-enter-ar', label: 'Enter AR', mode: 'immersive-ar', dx: '5%' }
    ];
    MODES.forEach(function(m) {
        let btn = document.getElementById(m.id);
        if (!btn) {
            btn = document.createElement('button');
            btn.id = m.id;
            btn.textContent = m.label;
            btn.style.cssText = 'position:fixed;left:50%;top:50%;transform:translate(' + m.dx + ',-50%);' +
                                'padding:16px 28px;font-size:20px;z-index:9999;cursor:pointer;display:none;';
            document.body.appendChild(btn);
        }
        const entry = { btn: btn, supported: false };
        buttons.push(entry);
        btn.onclick = function() { startSession(m.mode); };
        if (navigator.xr && navigator.xr.isSessionSupported) {
            navigator.xr.isSessionSupported(m.mode).then(function(ok) { entry.supported = ok; showButtons(); });
        }
    });
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
// clang-format on

extern "C" EMSCRIPTEN_KEEPALIVE void prev_webxr_dispatch_frame(void* self)
{
    if (self) {
        static_cast<prev::xr::web_xr::core::WebXrCore*>(self)->DispatchFrame();
    }
}

namespace prev::xr::web_xr::core {
WebXrCore::WebXrCore()
{
    if (!prev_webxr_is_supported()) {
        LOGW("WebXr: navigator.xr unavailable - WebXR is not supported in this browser");
    }
}

void WebXrCore::CreateSession(void* nativeDevice)
{
    prev_webxr_install_enter_buttons(this, nativeDevice);
    LOGI("WebXr: 'Enter VR' / 'Enter AR' buttons installed - click one to start the immersive session");
}

void WebXrCore::DestroySession()
{
    prev_webxr_end_session();
}

bool WebXrCore::IsSessionRunning() const
{
    return prev_webxr_is_running() != 0;
}

float WebXrCore::GetCurrentDeltaTime() const
{
    return static_cast<float>(prev_webxr_delta_time());
}

void WebXrCore::RunFrameLoop(const std::function<bool()>& tick)
{
    // Arm only; the rAF loop starts from the 'Enter VR' click (immersive sessions need a user gesture).
    m_frameCallback = tick;
}

void WebXrCore::DispatchFrame()
{
    // Do NOT reassign m_frameCallback from in here - it is the callable currently executing.
    if (m_frameCallback && !m_frameCallback()) {
        prev_webxr_stop_frame_loop();
    }
}
} // namespace prev::xr::web_xr::core

#endif
