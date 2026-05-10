#include "TestApp.h"
#include <prev/common/Logger.h>

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

#ifdef __EMSCRIPTEN__
    // On Emscripten, try/catch around Asyncify code is broken:
    // C++ exceptions thrown after an Asyncify suspend/resume cannot be caught
    // by a try/catch that was established before the suspension.
    try {
#endif
        prev_test::TestApp app{ config };
        app.Init();
        app.Run();
        app.ShutDown();
#ifdef __EMSCRIPTEN__
    } catch (const std::runtime_error& err) {
        LOGE("Unhandled error: %s", err.what());
    }
#endif

    return 0;
}
