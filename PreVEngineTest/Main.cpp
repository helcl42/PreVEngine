#include "TestApp.h"

int main(int argc, char* argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0); // avoid buffering

    prev::scene::SceneConfig sceneConfig{};
    sceneConfig.framesInFlight = 3;
    sceneConfig.VSync = true;
    sceneConfig.samplesCount = 4;

    prev::core::EngineConfig config{};
    config.appName = "Test App";
    config.fullScreen = false;
    config.validation = true;
    config.windowPosition = { 40, 40 };
    config.windowSize = { 1920, 1080 };
    config.sceneConfig = sceneConfig;

    prev_test::TestApp app(config);
    app.Init();
    app.Run();
    app.ShutDown();

    return 0;
}
