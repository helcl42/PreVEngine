#include "ShadowsCascade.h"

namespace prev_test::component::shadow {
void ShadowsCascade::Destroy(VkDevice device)
{
    vkDestroyImageView(device, imageView, nullptr);
    vkDestroyFramebuffer(device, frameBuffer, nullptr);
}

glm::mat4 ShadowsCascade::GetBiasedViewProjectionMatrix() const
{
    static const glm::mat4 biasMat(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f);

    return biasMat * projectionMatrix * viewMatrix;
}
} // namespace prev_test::component::shadow