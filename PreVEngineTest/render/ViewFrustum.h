#ifndef __VIEW_FRUSTUM_H__
#define __VIEW_FRUSTUM_H__

#include <prev/common/Common.h>

namespace prev_test::render {
class ViewFrustum {
public:
    ViewFrustum(const float verticalFov, const float nearClippingPlane, const float farClippingPlane);

    ~ViewFrustum() = default;

public:
    glm::mat4 CreateProjectionMatrix(const uint32_t w, const uint32_t h) const;

    glm::mat4 CreateProjectionMatrix(const float aspectRatio) const;

public:
    float GetVerticalFov() const; // vertical in degs

    void SetVerticalFov(float fov); // vertical in degs

    float GetNearClippingPlane() const;

    void SetNearClippingPlane(float nearClippingPlane);

    float GetFarClippingPlane() const;

    void SetFarClippingPlane(float farClippingPlane);

private:
    float m_verticalFov;

    float m_nearClippingPlane;

    float m_farClippingPlane;
};
} // namespace prev_test::render

#endif // !__VIEW_FRUSTUM_H__
