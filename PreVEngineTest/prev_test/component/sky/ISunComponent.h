#ifndef __ISUN_COMPONENT_H__
#define __ISUN_COMPONENT_H__

#include "Flare.h"

#include "../../render/IMaterial.h"
#include "../../render/IModel.h"

#include <prev/scene/component/IComponent.h>

namespace prev_test::component::sky {
class ISunComponent : public prev::scene::component::IComponent {
public:
    virtual std::shared_ptr<prev_test::render::IMaterial> GetMaterial() const = 0;

    virtual const Flare& GetFlare() const = 0;

    virtual std::shared_ptr<prev_test::render::IModel> GetModel() const = 0;

    virtual glm::vec2 ComputeFlarePosition(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& eyePosition, const glm::vec3& sunPosition) const = 0;

public:
    virtual ~ISunComponent() = default;
};
} // namespace prev_test::component::sky

#endif // !__ISUN_COMPONENT_H__
