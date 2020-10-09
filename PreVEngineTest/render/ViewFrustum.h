#ifndef __VIEW_FRUSTUM_H__
#define __VIEW_FRUSTUM_H__

#include <prev/common/Common.h>

namespace prev_test::render {
class ViewFrustum {
public:
    ViewFrustum(const float fov, const float nCp, const float fCp);

    ~ViewFrustum() = default;

public:
    glm::mat4 CreateProjectionMatrix(const uint32_t w, const uint32_t h) const;

    glm::mat4 CreateProjectionMatrix(const float aspectRatio) const;

public:
    float GetFov() const; // vertical in degs

    void SetFov(float fov); // vertical in degs

    float GetNearClippingPlane() const;

    void SetNearClippingPlane(float nearCP);

    float GetFarClippingPlane() const;

    void SetFarClippingPlane(float farCP);

private:
    float m_fov;

    float m_nearClippingPlane;

    float m_farClippingPlane;
};
} // namespace prev_test::render

#endif // !__VIEW_FRUSTUM_H__
