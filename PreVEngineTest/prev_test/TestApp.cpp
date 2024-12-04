#include "TestApp.h"

#include <prev/scene/Scene.h>

#include "render/renderer/MasterRenderer.h"
#include "scene/Root.h"

namespace prev_test {
TestApp::TestApp(const prev::core::engine::Config& config)
    : prev::App{ config }
{
}

std::shared_ptr<prev::scene::IScene> TestApp::CreateScene() const
{
    return std::make_shared<prev::scene::Scene>(std::make_shared<scene::Root>(*this->m_engine->GetDevice(), *this->m_engine->GetAllocator(), this->m_engine->GetViewCount()));
}

std::shared_ptr<prev::render::IRootRenderer> TestApp::CreateRootRenderer() const
{
    return std::make_shared<prev_test::render::renderer::MasterRenderer>(*this->m_engine->GetDevice(), *this->m_engine->GetAllocator(), *this->m_engine->GetRenderPass(), this->m_engine->GetSwapchain()->GetImageCount(), this->m_engine->GetViewCount());
}
} // namespace prev_test