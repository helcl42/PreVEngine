#include "TestApp.h"

#include <prev/event/EventChannel.h>
#include <prev/scene/Scene.h>
#include <prev/window/WindowEvents.h>

#include "render/renderer/MasterRenderer.h"
#include "scene/Root.h"

namespace prev_test {
TestApp::TestApp(const prev::core::engine::Config& config)
    : prev::App{ config }
{
}

void TestApp::operator()(const prev::input::keyboard::KeyEvent& keyEvent)
{
    if (keyEvent.action == prev::input::keyboard::KeyActionType::PRESS && keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_Escape) {
        prev::event::EventChannel::Post(prev::window::WindowCloseRequest{});
    }
}

std::unique_ptr<prev::scene::IScene> TestApp::CreateScene() const
{
    return std::make_unique<prev::scene::Scene>(std::make_shared<scene::Root>(this->m_engine->GetDevice(), this->m_engine->GetViewCount()));
}

std::unique_ptr<prev::render::IRootRenderer> TestApp::CreateRootRenderer() const
{
    return std::make_unique<prev_test::render::renderer::MasterRenderer>(this->m_engine->GetDevice(), this->m_engine->GetRenderPass(), this->m_engine->GetScene(), this->m_engine->GetSwapchain().GetImageCount(), this->m_engine->GetViewCount());
}
} // namespace prev_test