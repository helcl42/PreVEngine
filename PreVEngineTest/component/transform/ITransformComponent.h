#ifndef __ITRANSFORM_COMPONENT_H__
#define __ITRANSFORM_COMPONENT_H__

#include <prev/common/Common.h>

namespace prev_test::component::transform {
class ITransformComponent {
public:
    virtual void Update(float deltaTime) = 0;

    virtual std::shared_ptr<ITransformComponent> GetParent() const = 0;

    virtual void SetParent(const std::shared_ptr<ITransformComponent>& parent) = 0;

    virtual void Rotate(const glm::quat& rotationDiff) = 0;

    virtual void Translate(const glm::vec3& positionDiff) = 0;

    virtual void Scale(const glm::vec3& scaleDiff) = 0;

    virtual glm::quat GetOrientation() const = 0;

    virtual glm::vec3 GetPosition() const = 0;

    virtual glm::vec3 GetScale() const = 0;

    virtual void SetOrientation(const glm::quat& orientation) = 0;

    virtual void SetPosition(const glm::vec3& position) = 0;

    virtual void SetScale(const glm::vec3& scale) = 0;

    virtual glm::mat4 GetTransform() const = 0;

    virtual glm::mat4 GetTransformScaled() const = 0;

    virtual glm::mat4 GetWorldTransform() const = 0;

    virtual glm::mat4 GetWorldTransformScaled() const = 0;

    virtual glm::vec3 GetScaler() const = 0;

    virtual bool IsRoot() const = 0;

public:
    virtual ~ITransformComponent() = default;
};
}

#endif // !__ITRANSFORM_COMPONENT_H__
