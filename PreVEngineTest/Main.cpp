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

int main(int argc, char* argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0); // avoid buffering

    auto config = std::make_shared<prev::core::EngineConfig>();

    prev_test::TestApp<prev_test::SceneNodeFlags> app(config);
    app.Init();
    app.Run();
    app.ShutDown();

    return 0;
}
