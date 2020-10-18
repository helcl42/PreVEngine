#ifndef __APP_H__
#define __APP_H__

#include "core/Engine.h"
#include "render/IRenderer.h"
#include "scene/graph/ISceneNode.h"

namespace prev {
template <typename NodeFlagsType>
class App {
protected:
    std::unique_ptr<prev::core::Engine<NodeFlagsType> > m_engine;

public:
    App(const std::shared_ptr<prev::core::EngineConfig>& config)
        : m_engine(std::make_unique<prev::core::Engine<NodeFlagsType> >(config))
    {
    }

    virtual ~App() = default;

protected:
    virtual std::shared_ptr<prev::scene::graph::ISceneNode<NodeFlagsType> > CreateRootNode() const = 0;

    virtual std::shared_ptr<prev::render::IRenderer<NodeFlagsType, prev::render::DefaultRenderContextUserData> > CreateRootRenderer() const = 0;

public:
    void Init()
    {
        m_engine->Init();

        m_engine->InitScene();

        auto root = CreateRootNode();
        m_engine->InitSceneGraph(root);

        auto rootRenderer = CreateRootRenderer();
        m_engine->InitRenderer(rootRenderer);
    }

    void Run()
    {
        m_engine->MainLoop();
    }

    void ShutDown()
    {
        m_engine->ShutDown();
    }
};
} // namespace prev

#endif