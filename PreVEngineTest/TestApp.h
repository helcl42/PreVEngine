#ifndef __TEST_APP_H__
#define __TEST_APP_H__

#include "render/renderer/MasterRenderer.h"
#include "scene/Root.h"

#include <prev/App.h>

namespace prev_test {
class TestApp final : public prev::App {
public:
    TestApp(const std::shared_ptr<prev::core::EngineConfig>& config)
        : prev::App(config)
    {
    }

    ~TestApp() = default;

protected:
    std::shared_ptr<prev::scene::graph::ISceneNode> CreateRootNode() const override
    {
        return std::make_shared<scene::Root>();
    }

    std::shared_ptr<prev::render::IRenderer<prev::render::DefaultRenderContextUserData> > CreateRootRenderer() const override
    {
        auto scene = this->m_engine->GetScene();
        return std::make_shared<prev_test::render::renderer::MasterRenderer>(scene->GetRenderPass(), scene->GetSwapchain());
    }
};
} // namespace prev_test

#endif // !__PREV_TEST_APP_H__
