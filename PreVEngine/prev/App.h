#ifndef __APP_H__
#define __APP_H__

#include "core/Engine.h"
#include "render/IRootRenderer.h"
#include "scene/IScene.h"

namespace prev {
class App {
public:
    App(const prev::core::EngineConfig& config);

    virtual ~App() = default;

public:
    void Init();

    void Run();

    void ShutDown();

protected:
    virtual std::shared_ptr<prev::scene::IScene> CreateScene() const = 0;

    virtual std::shared_ptr<prev::render::IRootRenderer> CreateRootRenderer() const = 0;

protected:
    std::unique_ptr<prev::core::Engine> m_engine{};
};
} // namespace prev

#endif