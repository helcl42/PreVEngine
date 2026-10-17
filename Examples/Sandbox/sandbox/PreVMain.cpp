#include "SandboxApp.h"

#include <prev/common/Logger.h>

#include <cstdio>
#include <cstring>
#include <string>
#include <utility>

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
    config.colorManaged = true;
    return config;
}

bool ParseRenderBackend(const std::string& name, prev::core::engine::RenderBackend& outRenderBackend)
{
    const std::pair<const char*, prev::core::engine::RenderBackend> renderBackends[]{
        { "vulkan", prev::core::engine::RenderBackend::Vulkan },
        { "webgpu", prev::core::engine::RenderBackend::WebGPU }
    };
    for (const auto& [backendName, renderBackend] : renderBackends) {
        if (name == backendName) {
            outRenderBackend = renderBackend;
            return true;
        }
    }
    return false;
}

bool ParseWindowSystem(const std::string& name, prev::window::WindowSystem& outWindowSystem)
{
    const std::pair<const char*, prev::window::WindowSystem> windowSystems[]{
        { "auto", prev::window::WindowSystem::Auto },
        { "headless", prev::window::WindowSystem::Headless },
        { "win32", prev::window::WindowSystem::Win32 },
        { "macos", prev::window::WindowSystem::MacOS },
        { "ios", prev::window::WindowSystem::IOS },
        { "android", prev::window::WindowSystem::Android },
        { "emscripten", prev::window::WindowSystem::Emscripten },
        { "wayland", prev::window::WindowSystem::Wayland },
        { "xcb", prev::window::WindowSystem::Xcb },
        { "xlib", prev::window::WindowSystem::Xlib }
    };
    for (const auto& [systemName, windowSystem] : windowSystems) {
        if (name == systemName) {
            outWindowSystem = windowSystem;
            return true;
        }
    }
    return false;
}

void PrintUsage(const char* programName)
{
    std::printf("Usage: %s [options]\n", programName);
    std::printf("Options:\n");
    std::printf("  --backend <string>     Render backend: vulkan, webgpu (default: platform)\n");
    std::printf("  --window-system <string>  Window system: auto, headless, win32, macos, ios, android, emscripten, wayland, xcb, xlib (default: auto)\n");
    std::printf("  --sample-count <int>   MSAA sample count (default: 4)\n");
    std::printf("  --vsync <0|1>          Enable VSync (default: 1)\n");
    std::printf("  --color-managed <0|1>  Linear rendering + sRGB present (default: 1)\n");
    std::printf("  --ar                   Start in AR (passthrough) mode on XR builds (default: VR)\n");
    std::printf("  --help                 Print this help message\n");
}

bool ParseArgs(int argc, char** argv, prev::core::engine::Config& config)
{
    config = GetDefaultConfig();

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--backend") == 0 && i + 1 < argc) {
            const std::string backend{ argv[++i] };
            if (!ParseRenderBackend(backend, config.renderBackend)) {
                std::printf("Unknown backend: %s\n", backend.c_str());
                PrintUsage(argv[0]);
                return false;
            }
        } else if (std::strcmp(argv[i], "--window-system") == 0 && i + 1 < argc) {
            const std::string windowSystem{ argv[++i] };
            if (!ParseWindowSystem(windowSystem, config.windowSystem)) {
                std::printf("Unknown window system: %s\n", windowSystem.c_str());
                PrintUsage(argv[0]);
                return false;
            }
        } else if (std::strcmp(argv[i], "--sample-count") == 0 && i + 1 < argc) {
            config.samplesCount = static_cast<uint32_t>(std::stoi(argv[++i]));
        } else if (std::strcmp(argv[i], "--vsync") == 0 && i + 1 < argc) {
            config.VSync = std::stoi(argv[++i]) != 0;
        } else if (std::strcmp(argv[i], "--color-managed") == 0 && i + 1 < argc) {
            config.colorManaged = std::stoi(argv[++i]) != 0;
        } else if (std::strcmp(argv[i], "--ar") == 0) {
            config.xrMode = prev::core::engine::XrMode::AR;
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
