#ifndef __ITERRAIN_COMPONENT_H__
#define __ITERRAIN_COMPONENT_H__

#include "../../render/IMaterial.h"
#include "../../render/IModel.h"

#include "HeightMapInfo.h"

namespace prev_test::component::terrain {
struct VertexData {
    std::vector<glm::vec3> vertices;

    std::vector<glm::vec2> textureCoords;

    std::vector<glm::vec3> normals;

    std::vector<glm::vec3> tangents;

    std::vector<glm::vec3> biTangents;

    std::vector<uint32_t> indices;
};

class ITerrainComponenet {
public:
    virtual std::shared_ptr<prev_test::render::IModel> GetModel() const = 0;

    virtual std::vector<std::shared_ptr<prev_test::render::IMaterial>> GetMaterials() const = 0; // TODO make pack of materials controlled by height

    virtual bool GetHeightAt(const glm::vec3& position, float& outHeight) const = 0;

    virtual std::shared_ptr<VertexData> GetVertexData() const = 0;

    virtual std::shared_ptr<HeightMapInfo> GetHeightMapInfo() const = 0;

    virtual const glm::vec3& GetPosition() const = 0;

    virtual int GetGridX() const = 0;

    virtual int GetGridZ() const = 0;

    virtual std::vector<float> GetHeightSteps() const = 0;

    virtual float GetTransitionRange() const = 0;

public:
    virtual ~ITerrainComponenet() = default;
};
} // namespace prev_test::component::terrain

#endif // !__ITERRAIN_COMPONENT_H__
