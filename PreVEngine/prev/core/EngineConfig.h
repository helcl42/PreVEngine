#ifndef __ENGINE_CONFIG_H__
#define __ENGINE_CONFIG_H__

#include "../common/Common.h"
#include "../scene/SceneConfig.h"

namespace prev::core {
struct EngineConfig {
    bool validation{ true };

    std::string appName{ "PreVEngine - Demo" };

    bool fullScreen{ false };

    glm::ivec2 windowSize{ 1920, 1080 };

    glm::ivec2 windowPosition{ 40, 40 };

    std::shared_ptr<prev::scene::SceneConfig> sceneConfig{ std::make_shared<prev::scene::SceneConfig>() };
};
} // namespace prev::core

#endif // !__ENGINE_CONFIG_H__
