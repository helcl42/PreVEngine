#ifndef __ENGINE_CONFIG_H__
#define __ENGINE_CONFIG_H__

#include "../../common/Common.h"

namespace prev::core::engine {
struct Config {
    bool validation{ true };

    std::string appName{ "PreVEngine - Demo" };

    bool headless{ false };

    bool fullScreen{ false };

    glm::ivec2 windowSize{ 1920, 1080 };

    glm::ivec2 windowPosition{ 40, 40 };

    bool VSync{ true };

    uint32_t swapchainFrameCount{ 3 };

    uint32_t maxFramesInFlight{ 0 }; // 0 = match swapchain image count

    uint32_t samplesCount{ 1 };

    int32_t gpuIndex{ -1 };
};
} // namespace prev::core::engine

#endif // !__ENGINE_CONFIG_H__
