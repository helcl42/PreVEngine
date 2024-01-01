#include "ViewFrustum.h"

#include "../General.h"

#include <prev/util/MathUtils.h>

namespace prev_test::render {
ViewFrustum::ViewFrustum(const float verticalFov, const float nearClippingPlane, const float farClippingPlane)
    : m_verticalFov(verticalFov)
{
    if constexpr (REVERSE_DEPTH) {
        m_nearClippingPlane = farClippingPlane;
        m_farClippingPlane = nearClippingPlane;
    } else {
        m_nearClippingPlane = nearClippingPlane;
        m_farClippingPlane = farClippingPlane;
    }
}

glm::mat4 ViewFrustum::CreateProjectionMatrix(const uint32_t w, const uint32_t h) const
{
    const float aspectRatio{ static_cast<float>(w) / static_cast<float>(h) };
    return CreateProjectionMatrix(aspectRatio);
}

glm::mat4 ViewFrustum::CreateProjectionMatrix(const float aspectRatio) const
{

    return prev::util::math::CreatePerspectiveProjectionMatrix(aspectRatio, m_verticalFov, m_nearClippingPlane, m_farClippingPlane);
}

float ViewFrustum::GetVerticalFov() const
{
    return m_verticalFov;
}

void ViewFrustum::SetVerticalFov(float fov)
{
    m_verticalFov = fov;
}

float ViewFrustum::GetNearClippingPlane() const
{
    return m_nearClippingPlane;
}

void ViewFrustum::SetNearClippingPlane(float nearClippingPlane)
{
    m_nearClippingPlane = nearClippingPlane;
}

float ViewFrustum::GetFarClippingPlane() const
{
    return m_farClippingPlane;
}

void ViewFrustum::SetFarClippingPlane(float farClippingPlane)
{
    m_farClippingPlane = farClippingPlane;
}
} // namespace prev_test::render