#ifndef __WATER_COMPONENT_H__
#define __WATER_COMPONENT_H__

#include "IWaterComponent.h"

namespace prev_test::component::water {
class WaterComponent : public IWaterComponent {
public:
    WaterComponent(const int gridX, const int gridZ, const std::shared_ptr<prev_test::render::IMaterial>& material, const std::shared_ptr<prev_test::render::IModel>& model);

    ~WaterComponent() = default;

public:
    std::shared_ptr<prev_test::render::IMaterial> GetMaterial() const override;

    std::shared_ptr<prev_test::render::IModel> GetModel() const override;

    void Update(float deltaTime) override;

    float GetMoveFactor() const override;

    const glm::vec3& GetPosition() const override;

    int GetGridX() const override;

    int GetGridZ() const override;

private:
    const int m_gridX;

    const int m_gridZ;

    const glm::vec3 m_position;

    std::shared_ptr<prev_test::render::IMaterial> m_material;

    std::shared_ptr<prev_test::render::IModel> m_model;

    float m_moveFactor;
};
} // namespace prev_test::component::water

#endif