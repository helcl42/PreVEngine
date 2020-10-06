#include "TransformComponent.h"

#include <prev/util/MathUtils.h>

namespace prev_test::component::transform {
TransformComponent::TransformComponent()
    : m_worldTransform(1.0f)
    , m_position(glm::vec3(0.0f))
    , m_orientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f))
    , m_scaler(glm::vec3(1.0f))
{
}

TransformComponent::TransformComponent(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale)
    : m_worldTransform(1.0f)
    , m_position(position)
    , m_orientation(orientation)
    , m_scaler(scale)
{
}

void TransformComponent::Update(float deltaTime)
{
    if (auto parent = m_parent.lock()) { //This node has a parent...
        m_worldTransform = parent->GetWorldTransform() * GetTransform();
    } else { //Root node, world transform is local transform!
        m_worldTransform = GetTransform();
    }
}

void TransformComponent::SetParent(const std::shared_ptr<ITransformComponent>& parent)
{
    m_parent = parent;
}

std::shared_ptr<ITransformComponent> TransformComponent::GetParent() const
{
    return m_parent.lock();
}

void TransformComponent::Rotate(const glm::quat& rotationDiff)
{
    m_orientation = glm::normalize(m_orientation * rotationDiff);
}

void TransformComponent::Translate(const glm::vec3& positionDiff)
{
    m_position += positionDiff;
}

void TransformComponent::Scale(const glm::vec3& scaleDiff)
{
    m_scaler += scaleDiff;
}

glm::quat TransformComponent::GetOrientation() const
{
    return m_orientation;
}

glm::vec3 TransformComponent::GetPosition() const
{
    return m_position;
}

glm::vec3 TransformComponent::GetScale() const
{
    return m_scaler;
}

void TransformComponent::SetOrientation(const glm::quat& orientation)
{
    m_orientation = orientation;
}

void TransformComponent::SetPosition(const glm::vec3& position)
{
    m_position = position;
}

void TransformComponent::SetScale(const glm::vec3& scale)
{
    m_scaler = scale;
}

glm::mat4 TransformComponent::GetTransform() const
{
    return prev::util::MathUtil::CreateTransformationMatrix(m_position, m_orientation, glm::vec3(1.0f));
}

glm::mat4 TransformComponent::GetTransformScaled() const
{
    return prev::util::MathUtil::CreateTransformationMatrix(m_position, m_orientation, m_scaler);
}

glm::mat4 TransformComponent::GetWorldTransform() const
{
    return m_worldTransform;
}

glm::mat4 TransformComponent::GetWorldTransformScaled() const
{
    return glm::scale(GetWorldTransform(), m_scaler);
}

glm::vec3 TransformComponent::GetScaler() const
{
    return m_scaler;
}

bool TransformComponent::IsRoot() const
{
    return !m_parent.lock();
}
} // namespace prev_test::component::transform