#ifndef __SCENE_H__
#define __SCENE_H__

#include "../common/pattern/Singleton.h"
#include "../core/device/Device.h"
#include "../event/EventHandler.h"
#include "../render/IRenderer.h"
#include "../render/Swapchain.h"
#include "../window/WindowEvents.h"

#include "IScene.h"
#include "Scene.h"
#include "SceneConfig.h"
#include "graph/GraphTraversal.h"

namespace prev::scene {
class Scene : public IScene {
public:
    Scene(const SceneConfig& sceneConfig, const std::shared_ptr<prev::core::device::Device>& device, const std::shared_ptr<prev::core::memory::Allocator>& allocator, VkSurfaceKHR surface);

    virtual ~Scene() = default;

public:
    void Init() override;

    void InitSceneGraph(const std::shared_ptr<prev::scene::graph::ISceneNode>& rootNode) override;

    void InitRenderer(const std::shared_ptr<prev::render::IRenderer<prev::render::DefaultRenderContextUserData> >& rootRenderer) override;

    void Update(float deltaTime) override;

    void Render() override;

    void ShutDownRenderer() override;

    void ShutDownSceneGraph() override;

    void ShutDown() override;

public:
    std::shared_ptr<prev::render::Swapchain> GetSwapchain() const override;

    std::shared_ptr<prev::render::pass::RenderPass> GetRenderPass() const override;

    std::shared_ptr<prev::scene::graph::ISceneNode> GetRootNode() const override;

    std::shared_ptr<prev::render::IRenderer<prev::render::DefaultRenderContextUserData> > GetRootRenderer() const override;

public:
    void operator()(const prev::window::WindowResizeEvent& resizeEvent);

    void operator()(const prev::window::SurfaceChanged& surfaceChangedEvent);

private:
    void InitRenderPass();

    void InitSwapchain();

protected:
    const SceneConfig& m_config;

    std::shared_ptr<prev::core::device::Device> m_device;

    std::shared_ptr<prev::core::memory::Allocator> m_allocator;

    VkSurfaceKHR m_surface;

protected:
    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass;

    std::shared_ptr<prev::render::Swapchain> m_swapchain;

    std::shared_ptr<prev::scene::graph::ISceneNode> m_rootNode;

    std::shared_ptr<prev::render::IRenderer<prev::render::DefaultRenderContextUserData> > m_rootRenderer;

private:
    prev::event::EventHandler<Scene, prev::window::WindowResizeEvent> m_windowResizeEvent{ *this };

    prev::event::EventHandler<Scene, prev::window::SurfaceChanged> m_surfaceChangedEvent{ *this };
};
} // namespace prev::scene

#endif // !__SCENE_H__