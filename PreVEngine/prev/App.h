#ifndef __APP_H__
#define __APP_H__

#include "core/engine/Engine.h"
#include "render/IRootRenderer.h"
#include "scene/IScene.h"

namespace prev {
class App {
public:
    App(const prev::core::engine::Config& config);

    virtual ~App() = default;

public:
    void Init();

    void Run();

    void ShutDown();

protected:
    virtual std::unique_ptr<prev::scene::IScene> CreateScene() const = 0;

    virtual std::unique_ptr<prev::render::IRootRenderer> CreateRootRenderer() const = 0;

protected:
    std::unique_ptr<prev::core::engine::Engine> m_engine{};
};
} // namespace prev

#endif