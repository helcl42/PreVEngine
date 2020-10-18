#ifndef __ISCENE_H__
#define __ISCENE_H__

#include "../core/device/Device.h"
#include "../render/IRenderer.h"
#include "../render/Swapchain.h"
#include "graph/ISceneNode.h"

#include <memory>

namespace prev::scene {
template <typename NodeFlagsType>
class IScene {
public:
    virtual void Init() = 0;

    virtual void InitSceneGraph(const std::shared_ptr<prev::scene::graph::ISceneNode<NodeFlagsType> >& rootNode) = 0;

    virtual void InitRenderer(const std::shared_ptr<prev::render::IRenderer<NodeFlagsType, prev::render::DefaultRenderContextUserData> >& rootRenderer) = 0;

    virtual void Update(float deltaTime) = 0;

    virtual void Render() = 0;

    virtual void ShutDownRenderer() = 0;

    virtual void ShutDownSceneGraph() = 0;

    virtual void ShutDown() = 0;

    virtual std::shared_ptr<prev::scene::graph::ISceneNode<NodeFlagsType> > GetRootNode() const = 0;

    virtual std::shared_ptr<prev::render::IRenderer<NodeFlagsType, prev::render::DefaultRenderContextUserData> > GetRootRenderer() const = 0;

    virtual std::shared_ptr<prev::core::device::Device> GetDevice() const = 0;

    virtual std::shared_ptr<prev::render::Swapchain> GetSwapchain() const = 0;

    virtual std::shared_ptr<prev::render::pass::RenderPass> GetRenderPass() const = 0;

    virtual std::shared_ptr<prev::core::memory::Allocator> GetAllocator() const = 0;

public:
    virtual ~IScene() = default;
};
} // namespace prev::scene

#endif // __ISCENE_H__
