#ifndef __SHADOWS_CASCADE_H__
#define __SHADOWS_CASCADE_H__

#include <prev/core/Core.h>

namespace prev_test::component::shadow {
struct ShadowsCascadeRenderData {
    VkFramebuffer frameBuffer{};

    VkImageView imageView{};

    void Destroy(VkDevice device);
};

struct ShadowsCascadeFrameData {
    float startSplitDepth{};

    float endSplitDepth{};

    glm::mat4 viewMatrix{};

    glm::mat4 projectionMatrix{};

    glm::mat4 GetBiasedViewProjectionMatrix() const;
};
} // namespace prev_test::component::shadow

#endif // !__SHADOWS_CASCADE_H__
