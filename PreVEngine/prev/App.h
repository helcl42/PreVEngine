#ifndef __APP_H__
#define __APP_H__

#include "core/Engine.h"
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

public:
    void Init()
    {
        m_engine->Init();

        m_engine->InitScene();

        auto root = CreateRootNode();
        m_engine->GetScene()->SetRootNode(root);

        m_engine->InitSceneGraph();

        // TODO create renderer here ??
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