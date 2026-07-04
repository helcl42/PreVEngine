#include "SandboxApp.h"

#include "render/MasterRenderer.h"
#include "scene/Root.h"

#include <prev/event/EventChannel.h>
#include <prev/scene/Scene.h>
#include <prev/window/WindowEvents.h>

namespace sandbox {
SandboxApp::SandboxApp(const prev::core::engine::Config& config)
    : prev::App{ config }
{
#ifdef ENABLE_XR
    m_arEnabled = (config.xrMode == prev::core::engine::XrMode::AR);
#endif
}

void SandboxApp::operator()(const prev::input::keyboard::KeyEvent& keyEvent)
{
    if (keyEvent.action == prev::input::keyboard::KeyActionType::PRESS && keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_Escape) {
        prev::event::EventChannel::Post(prev::window::WindowCloseRequest{});
    }
#ifdef ENABLE_XR
    if (keyEvent.action == prev::input::keyboard::KeyActionType::PRESS && keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_P) {
        SetArEnabled(!m_arEnabled);
    }
#endif
}

#ifdef ENABLE_XR
void SandboxApp::operator()(const prev::xr::HandControllersEvent& event)
{
    constexpr float kPressThreshold{ 0.7f };
    const auto& left{ event.handControllers[0] };
    const auto& right{ event.handControllers[1] };
    const bool comboDown{ left.active && right.active && left.squeeze > kPressThreshold && right.squeeze > kPressThreshold };
    if (comboDown && !m_passthroughComboWasDown) {
        SetArEnabled(!m_arEnabled);
    }
    m_passthroughComboWasDown = comboDown;
}

void SandboxApp::SetArEnabled(bool enabled)
{
    prev::event::EventChannel::Post(prev::xr::XrPassthroughChangeRequestEvent{ enabled });
}

void SandboxApp::operator()(const prev::xr::XrPassthroughChangedEvent& event)
{
    m_arEnabled = event.enabled; // sync to the backend's actual (clamped) mode
}
#endif

std::unique_ptr<prev::scene::IScene> SandboxApp::CreateScene() const
{
    return std::make_unique<prev::scene::Scene>(std::make_shared<sandbox::scene::Root>(this->m_engine->GetDevice(), this->m_engine->GetViewCount()));
}

std::unique_ptr<prev::render::IRootRenderer> SandboxApp::CreateRootRenderer() const
{
    return std::make_unique<sandbox::render::MasterRenderer>(this->m_engine->GetDevice(), this->m_engine->GetRenderPass(), this->m_engine->GetScene());
}
} // namespace sandbox
