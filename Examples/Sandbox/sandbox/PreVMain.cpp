#include "SandboxApp.h"

#include <prev/common/Logger.h>

#include <cstdio>
#include <cstring>
#include <string>

namespace {
prev::core::engine::Config GetDefaultConfig()
{
    prev::core::engine::Config config{};
#ifdef __EMSCRIPTEN__
    config.renderBackend = prev::core::engine::RenderBackend::WebGPU;
#else
    config.renderBackend = prev::core::engine::RenderBackend::Vulkan;
#endif
    config.appName = "Sandbox";
    config.windowSize = { 1280, 720 };
    config.windowPosition = { 40, 40 };
    config.validation = true;
    config.VSync = true;
    config.samplesCount = 4;
    config.swapchainFrameCount = 3;
    config.maxFramesInFlight = 2;
    return config;
}

void PrintUsage(const char* programName)
{
    std::printf("Usage: %s [options]\n", programName);
    std::printf("Options:\n");
    std::printf("  --backend <string>     Render backend: vulkan, webgpu (default: platform)\n");
    std::printf("  --sample-count <int>   MSAA sample count (default: 4)\n");
    std::printf("  --vsync <0|1>          Enable VSync (default: 1)\n");
    std::printf("  --help                 Print this help message\n");
}

bool ParseArgs(int argc, char** argv, prev::core::engine::Config& config)
{
    config = GetDefaultConfig();

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--backend") == 0 && i + 1 < argc) {
            const std::string backend{ argv[++i] };
            if (backend == "vulkan") {
                config.renderBackend = prev::core::engine::RenderBackend::Vulkan;
            } else if (backend == "webgpu") {
                config.renderBackend = prev::core::engine::RenderBackend::WebGPU;
            } else {
                std::printf("Unknown backend: %s\n", backend.c_str());
                PrintUsage(argv[0]);
                return false;
            }
        } else if (std::strcmp(argv[i], "--sample-count") == 0 && i + 1 < argc) {
            config.samplesCount = static_cast<uint32_t>(std::stoi(argv[++i]));
        } else if (std::strcmp(argv[i], "--vsync") == 0 && i + 1 < argc) {
            config.VSync = std::stoi(argv[++i]) != 0;
        } else if (std::strcmp(argv[i], "--help") == 0) {
            PrintUsage(argv[0]);
            return false;
        } else {
            std::printf("Unknown option: %s\n", argv[i]);
            PrintUsage(argv[0]);
            return false;
        }
    }

    return true;
}
} // namespace

int PreVMain(int argc, char** argv)
{
    prev::core::engine::Config config{};
    if (!ParseArgs(argc, argv, config)) {
        return 1;
    }

#ifndef __EMSCRIPTEN__
    try {
#endif
        sandbox::SandboxApp app{ config };
        app.Init();
        app.Run();
        app.ShutDown();
#ifndef __EMSCRIPTEN__
    } catch (const std::exception& err) {
        LOGE("Fatal error: %s", err.what());
        return 1;
    }
#endif

    return 0;
}
