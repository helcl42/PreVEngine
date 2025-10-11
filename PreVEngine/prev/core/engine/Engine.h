#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "Config.h"

#include "impl/EngineImpl.h"

#include "../../render/IRootRenderer.h"
#include "../../render/pass/RenderPass.h"
#include "../../render/swapchain/ISwapchain.h"
#include "../../scene/IScene.h"
#include "../memory/Allocator.h"

namespace prev::core::engine {
class Engine final {
public:
    Engine(const Config& config);

    ~Engine();

public:
    void Init();

    void InitScene(std::unique_ptr<prev::scene::IScene> scene);

    void InitRenderer(std::unique_ptr<prev::render::IRootRenderer> rootRenderer);

    void MainLoop();

    void ShutDown();

public:
    prev::scene::IScene& GetScene() const;

    prev::render::IRootRenderer& GetRootRenderer() const;

    prev::render::swapchain::ISwapchain& GetSwapchain() const;

    prev::render::pass::RenderPass& GetRenderPass() const;

    prev::core::memory::Allocator& GetAllocator() const;

    prev::core::device::Device& GetDevice() const;

    const Config& GetConfig() const;

    uint32_t GetViewCount() const;

private:
    std::unique_ptr<prev::core::engine::impl::EngineImpl> m_engineImpl{};
};
} // namespace prev::core::engine

#endif // !__ENGINE_H__
