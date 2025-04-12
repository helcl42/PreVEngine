#ifndef __IWATER_COMPONENT_H__
#define __IWATER_COMPONENT_H__

#include "../../render/IMaterial.h"
#include "../../render/IModel.h"

#include <prev/scene/component/IComponent.h>

namespace prev_test::component::water {
class IWaterComponent : public prev::scene::component::IComponent {
public:
    virtual std::shared_ptr<prev_test::render::IModel> GetModel() const = 0;

    virtual std::shared_ptr<prev_test::render::IMaterial> GetMaterial() const = 0;

    virtual void Update(float deltaTime) = 0;

    virtual float GetMoveFactor() const = 0;

    virtual const glm::vec3& GetPosition() const = 0;

    virtual int GetGridX() const = 0;

    virtual int GetGridZ() const = 0;

public:
    virtual ~IWaterComponent() = default;
};
} // namespace prev_test::component::water

#endif // !__IWATER_COMPONENT_H__
