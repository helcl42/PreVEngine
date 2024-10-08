#include "App.h"

namespace prev {
App::App(const prev::core::EngineConfig& config)
    : m_engine{ std::make_unique<prev::core::Engine>(config) }
{
}

void App::Init()
{
    m_engine->Init();

    auto scene = CreateScene();
    m_engine->InitScene(scene);

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
