#ifndef __ISHADOWS_COMPONENT_H__
#define __ISHADOWS_COMPONENT_H__

#include "ShadowsCascade.h"

#include <prev/core/memory/image/IImageBuffer.h>
#include <prev/render/pass/RenderPass.h>

namespace prev_test::component::shadow {
class IShadowsComponent {
public:
    virtual void Update(const glm::vec3& lightDirection, const float nearClippingPlane, const float farClippingPlane, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) = 0;

    virtual std::shared_ptr<prev::render::pass::RenderPass> GetRenderPass() const = 0;

    virtual const ShadowsCascade& GetCascade(const size_t cascadeIndex) const = 0;

    virtual VkExtent2D GetExtent() const = 0;

    virtual std::shared_ptr<prev::core::memory::image::IImageBuffer> GetImageBuffer() const = 0;

public:
    virtual ~IShadowsComponent() = default;
};
} // namespace prev_test::component::shadow

#endif // !__ISHADOWS_COMPONENT_H__
