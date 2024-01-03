#ifndef __ISELECTABLE_COMPONENT_H__
#define __ISELECTABLE_COMPONENT_H__

#include <prev/common/Common.h>

namespace prev_test::component::ray_casting {
class ISelectableComponent {
public:
    virtual bool IsSelected() const = 0;

    virtual void SetSelected(const bool selected) = 0;

    virtual const glm::vec3& GetPostiion() const = 0;

    virtual void SetPosition(const glm::vec3& at) = 0;

    virtual void Reset() = 0;

public:
    virtual ~ISelectableComponent() = default;
};
} // namespace prev_test::component::ray_casting

#endif // !__ISELECTABLE_COMPONENT_H__
