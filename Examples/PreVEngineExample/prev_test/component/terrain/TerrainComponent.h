#ifndef __TERRAIN_COMPONENT_H__
#define __TERRAIN_COMPONENT_H__

#include "ITerrainComponent.h"
#include "TerrainCommon.h"

namespace prev_test::component::terrain {
class TerrainComponentFactory;

class TerrainComponent : public ITerrainComponent {
public:
    TerrainComponent(const int gridX, const int gridZ);

    ~TerrainComponent() = default;

public:
    std::shared_ptr<prev_test::render::IModel> GetModel() const override;

    std::vector<std::shared_ptr<prev_test::render::IMaterial>> GetMaterials() const override;

    bool GetHeightAt(const glm::vec3& position, float& outHeight) const override;

    const glm::vec3& GetPosition() const override;

    std::shared_ptr<HeightMapInfo> GetHeightMapInfo() const override;

    int GetGridX() const override;

    int GetGridZ() const override;

    std::vector<float> GetHeightSteps() const override;

    float GetTransitionRange() const override;

private:
    friend class TerrainComponentFactory;

private:
    const int m_gridX;

    const int m_gridZ;

    const glm::vec3 m_position;

    std::shared_ptr<HeightMapInfo> m_heightsInfo{};

    std::shared_ptr<prev_test::render::IModel> m_model{};

    std::vector<std::shared_ptr<prev_test::render::IMaterial>> m_materials;

    std::vector<float> m_heightSteps;

    float m_transitionRange{};
};
} // namespace prev_test::component::terrain

#endif // !__TERRAIN_COMPONENT_H__
