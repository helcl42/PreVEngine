#include "LightComponentFactory.h"
#include "LightComponent.h"

namespace prev_test::component::light {
std::unique_ptr<ILightComponent> LightComponentFactory::CreateLightCompoennt(const glm::vec3& position) const
{
    return std::make_unique<LightComponent>(position);
}

std::unique_ptr<ILightComponent> LightComponentFactory::CreateLightCompoennt(const glm::vec3& position, const glm::vec3& color, const glm::vec3& attenuation) const
{
    return std::make_unique<LightComponent>(position, color, attenuation);
}
} // namespace prev_test::component::light