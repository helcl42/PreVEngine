#include "TestApp.h"

int main(int argc, char* argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0); // avoid buffering

    prev::core::EngineConfig config{};
    config.appName = "Test App";
    config.fullScreen = false;
    config.validation = true;
    config.windowPosition = { 40, 40 };
    config.windowSize = { 1920, 1080 };
    config.framesInFlight = 3;
    config.VSync = true;
    config.samplesCount = 4;

    prev_test::TestApp app(config);
    app.Init();
    app.Run();
    app.ShutDown();

    return 0;
}
