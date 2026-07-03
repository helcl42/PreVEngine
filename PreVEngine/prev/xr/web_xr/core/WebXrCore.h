#ifndef __WEB_XR_CORE_H__
#define __WEB_XR_CORE_H__

#ifdef ENABLE_WEBXR

#include <functional>

namespace prev::xr::web_xr::core {
// Session lifecycle + frame loop. Owns the shared JS state (Module.__prevWebXr); render/input only read it.
class WebXrCore final {
public:
    WebXrCore();

    ~WebXrCore() = default;

public:
    void CreateSession(void* nativeDevice);

    void DestroySession();

    bool IsSessionRunning() const;

    float GetCurrentDeltaTime() const;

    void RunFrameLoop(const std::function<bool()>& tick);

    // Invoked from the XR session's requestAnimationFrame.
    void DispatchFrame();

private:
    std::function<bool()> m_frameCallback{}; // engine tick; false => stop the loop
};
} // namespace prev::xr::web_xr::core

#endif

#endif
