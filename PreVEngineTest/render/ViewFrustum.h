#ifndef __VIEW_FRUSTUM_H__
#define __VIEW_FRUSTUM_H__

#include <prev/common/Common.h>

namespace prev_test::render {
class ViewFrustum {
public:
    ViewFrustum(const float fov, const float nCp, const float fCp)
        : m_fov(fov)
        , m_nearClippingPlane(nCp)
        , m_farClippingPlane(fCp)
    {
    }

    ~ViewFrustum() = default;

public:
    glm::mat4 CreateProjectionMatrix(const uint32_t w, const uint32_t h) const
    {
        const float aspectRatio = static_cast<float>(w) / static_cast<float>(h);
        return CreateProjectionMatrix(aspectRatio);
    }

    glm::mat4 CreateProjectionMatrix(const float aspectRatio) const
    {
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(m_fov), aspectRatio, m_nearClippingPlane, m_farClippingPlane);
        projectionMatrix[1][1] *= -1; // invert Y in clip coordinates

        return projectionMatrix;
    }

public:
    float GetFov() const // vertical in degs
    {
        return m_fov;
    }

    void SetFov(float fov) // vertical in degs
    {
        m_fov = fov;
    }

    float GetNearClippingPlane() const
    {
        return m_nearClippingPlane;
    }

    void SetNearClippingPlane(float nearCP)
    {
        m_nearClippingPlane = nearCP;
    }

    float GetFarClippingPlane() const
    {
        return m_farClippingPlane;
    }

    void SetFarClippingPlane(float farCP)
    {
        m_farClippingPlane = farCP;
    }

private:
    float m_fov;

    float m_nearClippingPlane;

    float m_farClippingPlane;
};
} // namespace prev_test::render

#endif // !__VIEW_FRUSTUM_H__
