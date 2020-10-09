#ifndef __LIGHT_COMPONENT_FACTORY_H__
#define __LIGHT_COMPONENT_FACTORY_H__

#include "ILightComponent.h"

namespace prev_test::component::light {
class LightComponentFactory final {
public:
    std::unique_ptr<ILightComponent> CreateLightCompoennt(const glm::vec3& position) const;

    std::unique_ptr<ILightComponent> CreateLightCompoennt(const glm::vec3& position, const glm::vec3& color, const glm::vec3& attenuation) const;
};
} // namespace prev_test::component::light

#endif // !__LIGHT_COMPONENT_FACTORY_H__
