#ifndef __SKY_COMPONENT_H__
#define __SKY_COMPONENT_H__

#include "ISkyComponent.h"

namespace prev_test::component::sky {
class SkyComponentFactory;

class SkyComponent : public ISkyComponent {
public:
    std::shared_ptr<prev_test::render::IModel> GetModel() const override;

    glm::vec3 GetBottomColor() const override;

    glm::vec3 GetTopColor() const override;

private:
    friend class SkyComponentFactory;

private:
    std::shared_ptr<prev_test::render::IModel> m_model;

    glm::vec3 m_bottomColor;

    glm::vec3 m_topColor;
};
} // namespace prev_test::component::sky

#endif // !__SKY_COMPONENT_H__