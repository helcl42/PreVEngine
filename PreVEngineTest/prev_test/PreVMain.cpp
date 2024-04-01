#include "TestApp.h"

int PreVMain(int argc, char** argv)
{
   prev::core::EngineConfig config{};
   config.appName = "Test App";
   config.fullScreen = false;
   config.validation = true;
   config.windowPosition = { 40, 40 };
   config.windowSize = { 1024, 768 };
   config.VSync = true;
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
