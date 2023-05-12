#ifndef __SHADOWS_CASCADE_H__
#define __SHADOWS_CASCADE_H__

#include <prev/common/Common.h>
#include <prev/core/instance/Validation.h>

namespace prev_test::component::shadow {
struct ShadowsCascade {
    VkFramebuffer frameBuffer;

    VkImageView imageView;

    float startSplitDepth;

    float endSplitDepth;

    glm::mat4 viewMatrix;

    glm::mat4 projectionMatrix;

    void Destroy(VkDevice device);

    glm::mat4 GetBiasedViewProjectionMatrix() const;
};
} // namespace prev_test::component::shadow

#endif // !__SHADOWS_CASCADE_H__
