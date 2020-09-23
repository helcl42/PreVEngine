#ifndef __IRENDERER_H__
#define __IRENDERER_H__

#include <prev/render/RenderContext.h>

#include "RenderContextUserData.h"

namespace prev_test::render::renderer {
template <typename UserDataType = DefaultRenderContextUserData>
class IRenderer {
public:
    virtual void Init() = 0;

    virtual void BeforeRender(const prev::render::RenderContext& renderContext, const UserDataType& renderContextUserData = UserDataType{}) = 0;

    virtual void PreRender(const prev::render::RenderContext& renderContext, const UserDataType& renderContextUserData = UserDataType{}) = 0;

    virtual void Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode<SceneNodeFlags> >& node, const UserDataType& renderContextUserData = UserDataType{}) = 0;

    virtual void PostRender(const prev::render::RenderContext& renderContext, const UserDataType& renderContextUserData = UserDataType{}) = 0;

    virtual void AfterRender(const prev::render::RenderContext& renderContext, const UserDataType& renderContextUserData = UserDataType{}) = 0;

    virtual void ShutDown() = 0;

public:
    virtual ~IRenderer() = default;
};
} // namespace prev_test::render::renderer

#endif // !__IRENDERER_H__
