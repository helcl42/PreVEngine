#ifndef __IRENDERER_H__
#define __IRENDERER_H__

#include "../scene/graph/ISceneNode.h"
#include "DefaultRenderContextUserData.h"
#include "RenderContext.h"

namespace prev::render {
template <typename UserDataType = DefaultRenderContextUserData>
class IRenderer {
public:
    virtual void Init() = 0;

    virtual void BeforeRender(const prev::render::RenderContext& renderContext, const UserDataType& renderContextUserData = UserDataType{}) = 0;

    virtual void PreRender(const prev::render::RenderContext& renderContext, const UserDataType& renderContextUserData = UserDataType{}) = 0;

    virtual void Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const UserDataType& renderContextUserData = UserDataType{}) = 0;

    virtual void PostRender(const prev::render::RenderContext& renderContext, const UserDataType& renderContextUserData = UserDataType{}) = 0;

    virtual void AfterRender(const prev::render::RenderContext& renderContext, const UserDataType& renderContextUserData = UserDataType{}) = 0;

    virtual void ShutDown() = 0;

public:
    virtual ~IRenderer() = default;
};
} // namespace prev::render

#endif // !__IRENDERER_H__
