#ifndef __ISKY_BOX_COMPONENT_H__
#define __ISKY_BOX_COMPONENT_H__

#include "../../render/IModel.h"
#include "../../render/IMaterial.h"

namespace prev_test::component::sky {
class ISkyBoxComponent {
public:
    virtual std::shared_ptr<prev_test::render::IModel> GetModel() const = 0;

    virtual std::shared_ptr<prev_test::render::IMaterial> GetMaterial() const = 0;

public:
    virtual ~ISkyBoxComponent() = default;
};
} // namespace prev_test::component::sky

#endif // !__ISKY_BOX_COMPONENT_H__
