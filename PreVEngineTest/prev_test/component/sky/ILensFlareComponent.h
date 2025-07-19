#ifndef __ILENS_FLARE_COMPONENT_H__
#define __ILENS_FLARE_COMPONENT_H__

#include "Flare.h"

#include "../../render/IMaterial.h"
#include "../../render/IModel.h"

#include <prev/scene/component/IComponent.h>

namespace prev_test::component::sky {
class ILensFlareComponent : public prev::scene::component::IComponent {
public:
    virtual const std::vector<Flare>& GetFlares() const = 0;

    virtual const std::vector<std::shared_ptr<prev_test::render::IMaterial>>& GetMaterials() const = 0;

    virtual std::shared_ptr<prev_test::render::IModel> GetModel() const = 0;

    virtual std::vector<glm::vec2> ComputeFlarePositions(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& eyePosition, const glm::vec3& sunPosition) const = 0;

public:
    virtual ~ILensFlareComponent() = default;
};
} // namespace prev_test::component::sky

#endif // !__ILENS_FLARE_COMPONENT_H__
