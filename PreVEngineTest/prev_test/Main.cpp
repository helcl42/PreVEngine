#include "TestApp.h"

int main(int argc, char** argv)
{
    prev::core::EngineConfig config{};
    config.appName = "Test App";
    config.fullScreen = false;
    config.validation = false;
    config.windowPosition = { 40, 40 };
    config.windowSize = { 1920, 1080 };
    config.VSync = false;
    config.samplesCount = 1;
    config.swapchainFrameCount = 3;

    try {
        prev_test::TestApp app{ config };
        app.Init();
        app.Run();
        app.ShutDown();
    } catch (const std::runtime_error& err) {
        LOGE("Unhandled error: %s", err.what());
    }

    return 0;
}
