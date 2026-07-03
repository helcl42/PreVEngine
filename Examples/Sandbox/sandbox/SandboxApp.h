#ifndef __SANDBOX_APP_H__
#define __SANDBOX_APP_H__

#include <prev/App.h>
#include <prev/event/EventHandler.h>
#include <prev/input/keyboard/KeyboardEvents.h>

namespace sandbox {
class SandboxApp final : public prev::App {
public:
    SandboxApp(const prev::core::engine::Config& config);

    ~SandboxApp() = default;

public:
    void operator()(const prev::input::keyboard::KeyEvent& keyEvent);

protected:
    std::unique_ptr<prev::scene::IScene> CreateScene() const override;

    std::unique_ptr<prev::render::IRootRenderer> CreateRootRenderer() const override;

private:
    prev::event::EventHandler<SandboxApp, prev::input::keyboard::KeyEvent> m_keyEventHandler{ *this };
};
} // namespace sandbox

#endif // !__SANDBOX_APP_H__
