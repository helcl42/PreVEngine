#ifndef __TRANSFORM_COMPONENT_H__
#define __TRANSFORM_COMPONENT_H__

#include "ITransformComponent.h"

namespace prev_test::component::transform {
class TransformComponent : public ITransformComponent {
public:
    TransformComponent();

    TransformComponent(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale);

    ~TransformComponent() = default;

public:
    void Update(float deltaTime) override;

    void SetParent(const std::shared_ptr<ITransformComponent>& parent) override;

    std::shared_ptr<ITransformComponent> GetParent() const override;

    void Rotate(const glm::quat& rotationDiff) override;

    void Translate(const glm::vec3& positionDiff) override;

    void Scale(const glm::vec3& scaleDiff) override;

    glm::quat GetOrientation() const override;

    glm::vec3 GetPosition() const override;

    glm::vec3 GetScale() const override;

    void SetOrientation(const glm::quat& orientation) override;

    void SetPosition(const glm::vec3& position) override;

    void SetScale(const glm::vec3& scale) override;

    glm::mat4 GetTransform() const override;

    glm::mat4 GetTransformScaled() const override;

    glm::mat4 GetWorldTransform() const override;

    glm::mat4 GetWorldTransformScaled() const override;

    glm::vec3 GetScaler() const override;

    bool IsRoot() const override;

private:
    std::weak_ptr<ITransformComponent> m_parent;

    glm::mat4 m_worldTransform;

    glm::vec3 m_position;

    glm::quat m_orientation;

    glm::vec3 m_scaler;
};
} // namespace prev_test::component::transform

#endif