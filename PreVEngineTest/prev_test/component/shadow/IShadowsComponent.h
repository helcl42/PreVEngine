#ifndef __ISHADOWS_COMPONENT_H__
#define __ISHADOWS_COMPONENT_H__

#include "ShadowsCascade.h"

#include "../../render/ViewFrustum.h"

#include <prev/render/buffer/ImageBuffer.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/scene/component/IComponent.h>

#include <memory>

namespace prev_test::component::shadow {
class IShadowsComponent : public prev::scene::component::IComponent {
public:
    virtual void Update(const glm::vec3& lightDirection, const prev_test::render::ViewFrustum& viewFurstum, const glm::mat4& viewMatrix) = 0;

    virtual std::shared_ptr<prev::render::pass::RenderPass> GetRenderPass() const = 0;

    virtual const ShadowsCascade& GetCascade(const size_t cascadeIndex) const = 0;

    virtual VkExtent2D GetExtent() const = 0;

    virtual std::shared_ptr<prev::render::buffer::ImageBuffer> GetImageBuffer() const = 0;

public:
    virtual ~IShadowsComponent() = default;
};
} // namespace prev_test::component::shadow

#endif // !__ISHADOWS_COMPONENT_H__
