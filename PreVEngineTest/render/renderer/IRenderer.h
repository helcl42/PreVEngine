#ifndef __IRENDERER_H__
#define __IRENDERER_H__

#include <prev/render/RenderContext.h>
#include <prev/scene/graph/ISceneNode.h>

namespace prev_test::render::renderer {
template <typename RenderContextType = prev::render::RenderContext>
class IRenderer {
public:
    virtual void Init() = 0;

    virtual void BeforeRender(const RenderContextType& renderContext) = 0;

    virtual void PreRender(const RenderContextType& renderContext) = 0;

    virtual void Render(const RenderContextType& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node) = 0;

    virtual void PostRender(const RenderContextType& renderContext) = 0;

    virtual void AfterRender(const RenderContextType& renderContext) = 0;

    virtual void ShutDown() = 0;

public:
    virtual ~IRenderer() = default;
};
} // namespace prev_test::render::renderer

#endif // !__IRENDERER_H__