#ifndef __ENGINE_IMPL_H__
#define __ENGINE_IMPL_H__

#include "../Config.h"

#include "../../Core.h"
#include "../../memory/Allocator.h"
#include "../../device/Device.h"

#include "../../../event/EventHandler.h"
#include "../../../window/WindowEvents.h"
#include "../../../scene/IScene.h"
#include "../../../render/ISwapchain.h"
#include "../../../render/pass/RenderPass.h"
#include "../../../render/IRootRenderer.h"
#include "../../../util/Utils.h"

#include <memory>

namespace prev::core::engine::impl {
class EngineImpl {
public:
    EngineImpl(const Config& config);

    virtual ~EngineImpl();

public:
    std::shared_ptr<prev::scene::IScene> GetScene() const;

    std::shared_ptr<prev::render::ISwapchain> GetSwapchain() const;

    std::shared_ptr<prev::render::pass::RenderPass> GetRenderPass() const;

    std::shared_ptr<prev::render::IRootRenderer> GetRootRenderer() const;

    std::shared_ptr<prev::core::memory::Allocator> GetAllocator() const;

    std::shared_ptr<prev::core::device::Device> GetDevice() const;

    const Config& GetConfig() const;

public:
    virtual void Init() = 0;

    virtual void ShutDown() = 0;

    virtual bool Update() = 0;

    virtual bool BeginFrame() = 0;

    virtual bool EndFrame() = 0;

    virtual uint32_t GetViewCount() const = 0;

    virtual float GetCurrentDeltaTime() const = 0;

    virtual VkExtent2D GetExtent() const = 0;

public:
    void InitScene(const std::shared_ptr<prev::scene::IScene>& scene);

    void InitRenderer(const std::shared_ptr<prev::render::IRootRenderer>& rootRenderer);

    void BeginMainLoop();

    void EndMainLoop();

    bool IsFocused() const;

public:
    void operator()(const prev::window::WindowChangeEvent& windowChangeEvent);

    void operator()(const prev::window::WindowResizeEvent& resizeEvent);

protected:
    void ResetTiming();

    void ResetWindow();

    void ResetSurface();

    void ResetAllocator();

protected:
    virtual void ResetInstance() = 0;

    virtual void ResetDevice() = 0;

    virtual void ResetRenderPass() = 0;

    virtual void ResetSwapchain() = 0;

protected:
    std::unique_ptr<prev::render::pass::RenderPass> CreateDefaultMultisampledRenderPass(const prev::core::device::Device& device, const VkSurfaceKHR surface, const VkSampleCountFlagBits sampleCount, const uint32_t viewCount, const bool storeColor, const bool storeDepth);

    std::unique_ptr<prev::render::pass::RenderPass> CreateDefaultRenderPass(const prev::core::device::Device& device, const VkSurfaceKHR surface, const uint32_t viewCount, const bool storeColor, const bool storeDepth);

    void UpdateFps();

private:
    prev::event::EventHandler<EngineImpl, prev::window::WindowChangeEvent> m_windowChangedHandler{ *this };

    prev::event::EventHandler<EngineImpl, prev::window::WindowResizeEvent> m_windowResizeEvent{ *this };

protected:
    const Config& m_config;

    std::unique_ptr<prev::util::Clock<float>> m_clock{};

    std::unique_ptr<prev::util::FPSCounter> m_fpsCounter{};

    std::unique_ptr<prev::core::instance::Instance> m_instance{};

    std::unique_ptr<prev::window::IWindow> m_window{};

    VkSurfaceKHR m_surface{};

    std::shared_ptr<prev::core::device::Device> m_device{};

    std::shared_ptr<prev::core::memory::Allocator> m_allocator{};

    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass{};

    std::shared_ptr<prev::render::ISwapchain> m_swapchain{};

    std::shared_ptr<prev::scene::IScene> m_scene{};

    std::shared_ptr<prev::render::IRootRenderer> m_rootRenderer{};
};
}

#endif