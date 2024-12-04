#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "Config.h"

#include "impl/EngineImpl.h"

#include "../memory/Allocator.h"
#include "../../scene/IScene.h"
#include "../../render/IRootRenderer.h"
#include "../../render/ISwapchain.h"
#include "../../render/pass/RenderPass.h"

namespace prev::core::engine {
class Engine final {
public:
    Engine(const Config& config);

    ~Engine();

public:
    void Init();

    void InitScene(const std::shared_ptr<prev::scene::IScene>& scene);

    void InitRenderer(const std::shared_ptr<prev::render::IRootRenderer>& rootRenderer);

    void MainLoop();

    void ShutDown();

public:
    std::shared_ptr<prev::scene::IScene> GetScene() const;

    std::shared_ptr<prev::render::ISwapchain> GetSwapchain() const;

    std::shared_ptr<prev::render::pass::RenderPass> GetRenderPass() const;

    std::shared_ptr<prev::render::IRootRenderer> GetRootRenderer() const;

    std::shared_ptr<prev::core::memory::Allocator> GetAllocator() const;

    std::shared_ptr<prev::core::device::Device> GetDevice() const;

    const Config& GetConfig() const;

    uint32_t GetViewCount() const;

private:
    std::unique_ptr<prev::core::engine::impl::EngineImpl> m_engineImpl{};
};
} // namespace prev::core

#endif // !__ENGINE_H__
