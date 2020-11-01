#include "TestApp.h"

int main(int argc, char* argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0); // avoid buffering

    auto sceneConfig = std::make_shared<prev::scene::SceneConfig>();
    sceneConfig->framesInFlight = 3;
    sceneConfig->VSync = true;

    auto config = std::make_shared<prev::core::EngineConfig>();
    config->appName = "Test App";
    config->fullScreen = false;
    config->validation = true;
    config->windowPosition = { 40, 40 };
    config->windowSize = { 1920, 1080 };
    config->sceneConfig = sceneConfig;

    prev_test::TestApp app(config);
    app.Init();
    app.Run();
    app.ShutDown();

    return 0;
}
