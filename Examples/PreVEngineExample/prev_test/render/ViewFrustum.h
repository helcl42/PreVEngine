#ifndef __VIEW_FRUSTUM_H__
#define __VIEW_FRUSTUM_H__

#include <prev/common/Common.h>

namespace prev_test::render {
class ViewFrustum {
public:
    ViewFrustum(const float verticalFov, const float aspectRatio, const float nearClippingPlane, const float farClippingPlane);

    ViewFrustum(const float angleFovLeft, const float angleFovRight, const float angleFovUp, const float angleFovDown, const float nearClippingPlane, const float farClippingPlane);

    ~ViewFrustum() = default;

public:
    glm::mat4 CreateProjectionMatrix() const;

public:
    float GetVerticalFov() const; // degs

    float GetHorizontalFov() const; // degs

    float GetNearClippingPlane() const;

    float GetFarClippingPlane() const;

    float GetClippingRange() const;

private:
    float m_angleFovLeft;

    float m_angleFovRight;

    float m_angleFovUp;

    float m_angleFovDown;

    float m_nearClippingPlane;

    float m_farClippingPlane;
};
} // namespace prev_test::render

#endif // !__VIEW_FRUSTUM_H__
