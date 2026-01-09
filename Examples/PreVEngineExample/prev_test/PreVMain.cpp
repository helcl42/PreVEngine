#include "TestApp.h"

int PreVMain(int argc, char** argv)
{
    prev::core::engine::Config config{};
    config.headless = false;
    config.appName = "Test App";
    config.fullScreen = false;
    config.validation = true;
    config.windowPosition = { 40, 40 };
    config.windowSize = { 1600, 900 };
    config.VSync = true;
    config.samplesCount = 4;
    config.swapchainFrameCount = 3;
    config.maxFramesInFlight = 2;

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
