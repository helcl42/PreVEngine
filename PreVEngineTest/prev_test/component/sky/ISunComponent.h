#ifndef __ISUN_COMPONENT_H__
#define __ISUN_COMPONENT_H__

#include "Flare.h"

#include "../../render/IModel.h"

namespace prev_test::component::sky {
class ISunComponent {
public:
    virtual void Update(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& eyePosition, const glm::vec3& sunPosition) = 0;

    virtual std::shared_ptr<Flare> GetFlare() const = 0;

    virtual std::shared_ptr<prev_test::render::IModel> GetModel() const = 0;

public:
    virtual ~ISunComponent() = default;
};
} // namespace prev_test::component::sky

#endif // !__ISUN_COMPONENT_H__
