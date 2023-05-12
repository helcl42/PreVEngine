#ifndef __APP_H__
#define __APP_H__

#include "core/Engine.h"
#include "render/IRenderer.h"
#include "scene/graph/ISceneNode.h"

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
    virtual std::shared_ptr<prev::scene::graph::ISceneNode> CreateRootNode() const = 0;

    virtual std::shared_ptr<prev::render::IRenderer<prev::render::DefaultRenderContextUserData>> CreateRootRenderer() const = 0;

protected:
    std::unique_ptr<prev::core::Engine> m_engine;
};
} // namespace prev

#endif