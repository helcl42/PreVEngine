#ifndef __APP_H__
#define __APP_H__

#include "core/Engine.h"

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
    virtual void OnEngineInit() = 0;

    virtual void OnSceneInit() = 0;

    virtual void OnSceneGraphInit() = 0;

public:
    void Init()
    {
        m_engine->Init();

        OnEngineInit();

        m_engine->InitScene();

        OnSceneInit();

        m_engine->InitSceneGraph();

        OnSceneGraphInit();
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