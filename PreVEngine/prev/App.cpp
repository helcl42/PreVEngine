#include "App.h"

namespace prev {
App::App(const prev::core::EngineConfig& config)
    : m_engine(std::make_unique<prev::core::Engine>(config))
{
}

void App::Init()
{
    m_engine->Init();

    m_engine->InitScene();

    auto root = CreateRootNode();
    m_engine->InitSceneGraph(root);

    auto rootRenderer = CreateRootRenderer();
    m_engine->InitRenderer(rootRenderer);
}

void App::Run()
{
    m_engine->MainLoop();
}

void App::ShutDown()
{
    m_engine->ShutDown();
}
} // namespace prev
