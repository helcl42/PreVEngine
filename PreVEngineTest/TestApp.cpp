#include "TestApp.h"

#include "render/renderer/MasterRenderer.h"
#include "scene/Root.h"

namespace prev_test {
TestApp::TestApp(const prev::core::EngineConfig& config)
    : prev::App(config)
{
}

std::shared_ptr<prev::scene::graph::ISceneNode> TestApp::CreateRootNode() const
{
    return std::make_shared<scene::Root>();
}

std::shared_ptr<prev::render::IRenderer<prev::render::DefaultRenderContextUserData> > TestApp::CreateRootRenderer() const
{
    auto scene = this->m_engine->GetScene();
    return std::make_shared<prev_test::render::renderer::MasterRenderer>(scene->GetRenderPass(), scene->GetSwapchain());
}
} // namespace prev_test