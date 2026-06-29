#ifndef __IROOT_RENDERER_H__
#define __IROOT_RENDERER_H__

#include "FrameSubmitSync.h"
#include "RenderContext.h"

#include "../scene/IScene.h"

namespace prev::render {
class IRootRenderer {
public:
    virtual void Init() = 0;

    virtual FrameSubmitSync Render(const prev::render::RenderContext& renderContext, const prev::scene::IScene& scene) = 0;

    virtual void ShutDown() = 0;

public:
    virtual ~IRootRenderer() = default;
};
} // namespace prev::render

#endif // !__IROOT_RENDERER_H__
