#ifndef __SANDBOX_APP_H__
#define __SANDBOX_APP_H__

#include <prev/App.h>
#include <prev/event/EventHandler.h>
#include <prev/input/keyboard/KeyboardEvents.h>

#ifdef ENABLE_XR
#include <prev/xr/XrEvents.h>
#endif

namespace sandbox {
class SandboxApp final : public prev::App {
public:
    SandboxApp(const prev::core::engine::Config& config);

    ~SandboxApp() = default;

public:
    void operator()(const prev::input::keyboard::KeyEvent& keyEvent);

#ifdef ENABLE_XR
    void operator()(const prev::xr::HandControllersEvent& event);

    void operator()(const prev::xr::XrPassthroughChangedEvent& event);
#endif

protected:
#ifdef ENABLE_XR
    void SetArEnabled(bool enabled);
#endif

    std::unique_ptr<prev::scene::IScene> CreateScene() const override;

    std::unique_ptr<prev::render::IRootRenderer> CreateRootRenderer() const override;

private:
    prev::event::EventHandler<SandboxApp, prev::input::keyboard::KeyEvent> m_keyEventHandler{ *this };

#ifdef ENABLE_XR
    prev::event::EventHandler<SandboxApp, prev::xr::HandControllersEvent> m_controllersHandler{ *this };

    prev::event::EventHandler<SandboxApp, prev::xr::XrPassthroughChangedEvent> m_passthroughChangedHandler{ *this };

    bool m_arEnabled{ false }; // actual mode; seeded from Config::xrMode, then synced from XrPassthroughChangedEvent

    bool m_passthroughComboWasDown{ false };
#endif
};
} // namespace sandbox

#endif // !__SANDBOX_APP_H__
