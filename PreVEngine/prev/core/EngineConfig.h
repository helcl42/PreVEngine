#ifndef __ENGINE_CONFIG_H__
#define __ENGINE_CONFIG_H__

#include "../common/Common.h"

namespace prev::core {
struct EngineConfig {
    bool validation{ true };

    std::string appName{ "PreVEngine - Demo" };

    bool fullScreen{ false };

    glm::ivec2 windowSize{ 1920, 1080 };

    glm::ivec2 windowPosition{ 40, 40 };

    bool VSync{ true };

    uint32_t framesInFlight{ 3 };

    uint32_t samplesCount{ 1 };
};
} // namespace prev::core

#endif // !__ENGINE_CONFIG_H__
