#ifndef __PREV_TEST_APP_H__
#define __PREV_TEST_APP_H__

#include "General.h"
#include "scene/Root.h"

#include <prev/App.h>

namespace prev_test {
template <typename NodeFlagsType>
class TestApp final : public prev::App<NodeFlagsType> {
public:
    TestApp(const std::shared_ptr<prev::core::EngineConfig>& config)
        : prev::App<NodeFlagsType>(config)
    {
    }

    ~TestApp() = default;

protected:
    void OnEngineInit() override
    {
    }

    void OnSceneInit() override
    {
        auto scene = this->m_engine->GetScene();

        auto rootNode = std::make_shared<scene::Root>(scene->GetRenderPass(), scene->GetSwapchain());

        scene->SetRootNode(rootNode);
    }

    void OnSceneGraphInit() override
    {
    }
};
} // namespace prev_test

#endif // !__PREV_TEST_APP_H__
