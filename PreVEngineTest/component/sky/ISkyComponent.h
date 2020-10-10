#ifndef __ISKY_COMPONENT_H__
#define __ISKY_COMPONENT_H__

#include "../../render/IModel.h"

namespace prev_test::component::sky {
class ISkyComponent {
public:
    virtual std::shared_ptr<prev_test::render::IModel> GetModel() const = 0;

    virtual glm::vec3 GetBottomColor() const = 0;

    virtual glm::vec3 GetTopColor() const = 0;

public:
    virtual ~ISkyComponent() = default;
};
} // namespace prev_test::component::sky

#endif // !__SKY_COMPONENT_H__
