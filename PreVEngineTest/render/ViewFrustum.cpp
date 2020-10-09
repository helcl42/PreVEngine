#include "ViewFrustum.h"

namespace prev_test::render {
ViewFrustum::ViewFrustum(const float fov, const float nCp, const float fCp)
    : m_fov(fov)
    , m_nearClippingPlane(nCp)
    , m_farClippingPlane(fCp)
{
}

glm::mat4 ViewFrustum::CreateProjectionMatrix(const uint32_t w, const uint32_t h) const
{
    const float aspectRatio = static_cast<float>(w) / static_cast<float>(h);
    return CreateProjectionMatrix(aspectRatio);
}

glm::mat4 ViewFrustum::CreateProjectionMatrix(const float aspectRatio) const
{
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(m_fov), aspectRatio, m_nearClippingPlane, m_farClippingPlane);
    projectionMatrix[1][1] *= -1; // invert Y in clip coordinates

    return projectionMatrix;
}

float ViewFrustum::GetFov() const
{
    return m_fov;
}

void ViewFrustum::SetFov(float fov)
{
    m_fov = fov;
}

float ViewFrustum::GetNearClippingPlane() const
{
    return m_nearClippingPlane;
}

void ViewFrustum::SetNearClippingPlane(float nearCP)
{
    m_nearClippingPlane = nearCP;
}

float ViewFrustum::GetFarClippingPlane() const
{
    return m_farClippingPlane;
}

void ViewFrustum::SetFarClippingPlane(float farCP)
{
    m_farClippingPlane = farCP;
}
} // namespace prev_test::render