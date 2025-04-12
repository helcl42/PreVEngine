#ifndef __IRENDER_COMPONENT_H__
#define __IRENDER_COMPONENT_H__

#include "../../render/IMaterial.h"
#include "../../render/IModel.h"

#include <prev/scene/component/IComponent.h>

#include <memory>

namespace prev_test::component::render {
class IRenderComponent : public prev::scene::component::IComponent {
public:
    virtual std::shared_ptr<prev_test::render::IModel> GetModel() const = 0;

    virtual std::shared_ptr<prev_test::render::IMaterial> GetMaterial(const uint32_t = 0) const = 0;

    virtual const std::vector<std::shared_ptr<prev_test::render::IMaterial>>& GetMaterials() const = 0;

    virtual bool CastsShadows() const = 0;

    virtual bool IsCastedByShadows() const = 0;

public:
    virtual ~IRenderComponent() = default;
};
} // namespace prev_test::component::render

#endif // !__IRENDER_COMPONENT_H__
