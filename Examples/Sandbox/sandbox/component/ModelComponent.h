#ifndef __SANDBOX_MODEL_COMPONENT_H__
#define __SANDBOX_MODEL_COMPONENT_H__

#include "../render/Model.h"

#include <prev/scene/component/IComponent.h>

#include <memory>

namespace sandbox::component {
// Holds the (shared) geometry a node renders. Multiple nodes can reference the same Model.
class ModelComponent final : public prev::scene::component::IComponent {
public:
    explicit ModelComponent(std::shared_ptr<sandbox::render::Model> model)
        : m_model{ std::move(model) }
    {
    }

    std::shared_ptr<sandbox::render::Model> GetModel() const
    {
        return m_model;
    }

private:
    std::shared_ptr<sandbox::render::Model> m_model;
};
} // namespace sandbox::component

#endif // !__SANDBOX_MODEL_COMPONENT_H__
