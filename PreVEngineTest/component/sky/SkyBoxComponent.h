#ifndef __SKY_BOX_COMPONENT_H__
#define __SKY_BOX_COMPONENT_H__

#include "ISkyBoxComponent.h"

namespace prev_test::component::sky {
class SkyBoxComponentFactory;

class SkyBoxComponent : public ISkyBoxComponent {
public:
    std::shared_ptr<prev_test::render::IModel> GetModel() const override;

    std::shared_ptr<prev_test::render::IMaterial> GetMaterial() const override;

private:
    friend class SkyBoxComponentFactory;

private:
    std::shared_ptr<prev_test::render::IModel> m_model;

    std::shared_ptr<prev_test::render::IMaterial> m_material;
};
} // namespace prev_test::component::sky

#endif // !__SKY_BOX_COMPONENT_H__
