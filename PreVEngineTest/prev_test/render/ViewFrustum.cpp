#include "ViewFrustum.h"

#include "../General.h"

#include <prev/util/MathUtils.h>

namespace prev_test::render {
namespace {
    float ScaleAngle(const float angle, const float scale)
    {
        return std::atan(std::tan(angle) * scale);
    }
} // namespace

ViewFrustum::ViewFrustum(const float verticalFov, const float aspectRatio, const float nearClippingPlane, const float farClippingPlane)
    : m_angleFovLeft{ ScaleAngle(-verticalFov / 2.0f, aspectRatio) }
    , m_angleFovRight{ ScaleAngle(verticalFov / 2.0f, aspectRatio) }
    , m_angleFovUp{ verticalFov / 2.0f }
    , m_angleFovDown{ -verticalFov / 2.0f }
    , m_nearClippingPlane{ nearClippingPlane }
    , m_farClippingPlane{ farClippingPlane }
{
}

ViewFrustum::ViewFrustum(const float angleFovLeft, const float angleFovRight, const float angleFovUp, const float angleFovDown, const float nearClippingPlane, const float farClippingPlane)
    : m_angleFovLeft{ angleFovLeft }
    , m_angleFovRight{ angleFovRight }
    , m_angleFovUp{ angleFovUp }
    , m_angleFovDown{ angleFovDown }
    , m_nearClippingPlane{ nearClippingPlane }
    , m_farClippingPlane{ farClippingPlane }
{
}

glm::mat4 ViewFrustum::CreateProjectionMatrix() const
{
    return prev::util::math::CreatePerspectiveProjectionMatrix(prev::util::math::Fov{ m_angleFovLeft, m_angleFovRight, m_angleFovUp, m_angleFovDown }, m_nearClippingPlane, m_farClippingPlane);
}

float ViewFrustum::GetVerticalFov() const
{
    return m_angleFovRight - m_angleFovLeft;
}

float ViewFrustum::GetHorizontalFov() const
{
    return m_angleFovDown - m_angleFovUp;
}

float ViewFrustum::GetNearClippingPlane() const
{
    return m_nearClippingPlane;
}

float ViewFrustum::GetFarClippingPlane() const
{
    return m_farClippingPlane;
}

float ViewFrustum::GetClippingRange() const
{
    return m_farClippingPlane - m_nearClippingPlane;
}
} // namespace prev_test::render
