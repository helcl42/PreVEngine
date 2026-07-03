#ifndef __WEB_XR_INPUT_H__
#define __WEB_XR_INPUT_H__

#ifdef ENABLE_WEBXR

namespace prev::xr::web_xr::input {
class WebXrInput final {
public:
    WebXrInput() = default;

    ~WebXrInput() = default;

public:
    void PollActions();

private:
    void HandleControllerActions();

    void HandleHandTrackingActions();
};
} // namespace prev::xr::web_xr::input

#endif

#endif
