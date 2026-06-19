#include "SandboxApp.h"

#include "render/MasterRenderer.h"
#include "scene/Root.h"

#include <prev/scene/Scene.h>

namespace sandbox {
SandboxApp::SandboxApp(const prev::core::engine::Config& config)
    : prev::App{ config }
{
}

std::unique_ptr<prev::scene::IScene> SandboxApp::CreateScene() const
{
    return std::make_unique<prev::scene::Scene>(std::make_shared<sandbox::scene::Root>(this->m_engine->GetDevice(), this->m_engine->GetViewCount()));
}

std::unique_ptr<prev::render::IRootRenderer> SandboxApp::CreateRootRenderer() const
{
    return std::make_unique<sandbox::render::MasterRenderer>(this->m_engine->GetDevice(), this->m_engine->GetRenderPass(), this->m_engine->GetScene());
}
} // namespace sandbox
