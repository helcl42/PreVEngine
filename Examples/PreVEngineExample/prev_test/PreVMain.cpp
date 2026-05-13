#include "TestApp.h"
#include <prev/common/Logger.h>

#include <cstring>
#include <string>

namespace {
void PrintUsage(const char* programName)
{
    printf("Usage: %s [options]\n", programName);
    printf("Options:\n");
    printf("  --width <int>          Window width (default: 1600)\n");
    printf("  --height <int>         Window height (default: 900)\n");
    printf("  --left <int>           Window X position (default: 40)\n");
    printf("  --top <int>            Window Y position (default: 40)\n");
    printf("  --fullscreen <0|1>     Enable fullscreen mode (default: 0)\n");
    printf("  --vsync <0|1>          Enable VSync (default: 1)\n");
    printf("  --validation <0|1>     Enable validation layers (default: 1)\n");
    printf("  --headless <0|1>       Run in headless mode (default: 0)\n");
    printf("  --sample-count <int>   MSAA sample count (default: 4)\n");
    printf("  --swapchain-images <int>  Swapchain image count (default: 3)\n");
    printf("  --frames-in-flight <int>  Max frames in flight (default: 2)\n");
    printf("  --gpu <int>            GPU index to use (default: auto)\n");
    printf("  --help                 Print this help message\n");
}

prev::core::engine::Config GetDefaultConfig()
{
    prev::core::engine::Config config{};
#ifdef __EMSCRIPTEN__
    config.renderBackend = prev::core::engine::RenderBackend::WebGPU;
#else
    config.renderBackend = prev::core::engine::RenderBackend::Vulkan;
#endif
    config.appName = "Test App";
    config.windowSize = { 1600, 900 };
    config.windowPosition = { 40, 40 };
    config.validation = true;
    config.VSync = true;
    config.samplesCount = 4;
    config.swapchainFrameCount = 3;
    config.maxFramesInFlight = 2;
    return config;
}

prev::core::engine::Config ParseArgs(int argc, char** argv)
{
    prev::core::engine::Config config{ GetDefaultConfig() };

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--width") == 0 && i + 1 < argc) {
            config.windowSize.x = std::stoi(argv[++i]);
        } else if (strcmp(argv[i], "--height") == 0 && i + 1 < argc) {
            config.windowSize.y = std::stoi(argv[++i]);
        } else if (strcmp(argv[i], "--left") == 0 && i + 1 < argc) {
            config.windowPosition.x = std::stoi(argv[++i]);
        } else if (strcmp(argv[i], "--top") == 0 && i + 1 < argc) {
            config.windowPosition.y = std::stoi(argv[++i]);
        } else if (strcmp(argv[i], "--fullscreen") == 0 && i + 1 < argc) {
            config.fullScreen = std::stoi(argv[++i]) != 0;
        } else if (strcmp(argv[i], "--vsync") == 0 && i + 1 < argc) {
            config.VSync = std::stoi(argv[++i]) != 0;
        } else if (strcmp(argv[i], "--validation") == 0 && i + 1 < argc) {
            config.validation = std::stoi(argv[++i]) != 0;
        } else if (strcmp(argv[i], "--headless") == 0 && i + 1 < argc) {
            config.headless = std::stoi(argv[++i]) != 0;
        } else if (strcmp(argv[i], "--sample-count") == 0 && i + 1 < argc) {
            config.samplesCount = static_cast<uint32_t>(std::stoi(argv[++i]));
        } else if (strcmp(argv[i], "--swapchain-images") == 0 && i + 1 < argc) {
            config.swapchainFrameCount = static_cast<uint32_t>(std::stoi(argv[++i]));
        } else if (strcmp(argv[i], "--frames-in-flight") == 0 && i + 1 < argc) {
            config.maxFramesInFlight = static_cast<uint32_t>(std::stoi(argv[++i]));
        } else if (strcmp(argv[i], "--gpu-index") == 0 && i + 1 < argc) {
            config.gpuIndex = std::stoi(argv[++i]);
        } else if (strcmp(argv[i], "--help") == 0) {
            PrintUsage(argv[0]);
            exit(0);
        } else {
            printf("Unknown option: %s\n", argv[i]);
            PrintUsage(argv[0]);
            exit(1);
        }
    }

    return config;
}
} // namespace

int PreVMain(int argc, char** argv)
{
    const auto config = ParseArgs(argc, argv);

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
