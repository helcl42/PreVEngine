#ifndef __SELECTABLE_COMPONENT_H__
#define __SELECTABLE_COMPONENT_H__

#include "ISelectableComponent.h"

namespace prev_test::component::ray_casting {
class SelectableComponent final : public ISelectableComponent {
public:
    bool IsSelected() const override;

    void SetSelected(const bool selected) override;

    const glm::vec3& GetPostiion() const override;

    void SetPosition(const glm::vec3& at) override;

    void Reset() override;

private:
    bool m_selected{ false };

    glm::vec3 m_position{ -std::numeric_limits<float>::max() };
};
} // namespace prev_test::component::ray_casting

#endif // !__SELECTABLE_COMPONENT_H__
