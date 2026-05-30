#ifndef __SHADOWS_CASCADE_H__
#define __SHADOWS_CASCADE_H__

#include <prev/core/Core.h>
#include <prev/render/buffer/ImageBufferView.h>
#include <prev/render/framebuffer/Framebuffer.h>

#include <memory>

namespace prev_test::component::shadow {
struct ShadowsCascadeRenderData {
    std::unique_ptr<prev::render::framebuffer::Framebuffer> framebuffer;

    std::unique_ptr<prev::render::buffer::ImageBufferView> textureView;
};

struct ShadowsCascadeFrameData {
    float startSplitDepth{};

    float endSplitDepth{};

    glm::mat4 viewMatrix{};

    glm::mat4 projectionMatrix{};

    glm::mat4 GetBiasedViewProjectionMatrix(bool flipY = false) const;
};
} // namespace prev_test::component::shadow

#endif // !__SHADOWS_CASCADE_H__
