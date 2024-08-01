#ifndef __MATH_UTILS_H__
#define __MATH_UTILS_H__

#include "../common/Common.h"

#include <vector>

namespace prev::util::math {

template <typename T>
inline constexpr T Clamp(T val, T min, T max)
{
    return val < min ? min : val > max ? max
                                       : val;
}

template <typename T>
inline T Log2(const T x)
{
    return static_cast<T>(log(x) / log(static_cast<T>(2)));
}

template <typename T>
inline T RoundUp(const T val, const T toDivBy)
{
    if (toDivBy <= static_cast<T>(0)) {
        return val;
    }
    if (val <= toDivBy) {
        return toDivBy;
    }
    return val + (toDivBy - (val % toDivBy));
}

template <typename T>
inline T RoundDown(const T val, const T toDivBy)
{
    if (toDivBy <= static_cast<T>(0)) {
        return val;
    }
    return val - (val % toDivBy);
}

template <class T>
inline constexpr T Absolute(T arg)
{
    return arg < 0 ? -arg : arg;
}

template <typename T>
inline constexpr T AlmostEqual(T value1, T value2, T epsilon = T(1e-7))
{
    return Absolute(value1 - value2) < epsilon;
}

template <typename T>
inline constexpr T AlmostZero(T value1, T epsilon = T(1e-7))
{
    return AlmostEqual(value1, static_cast<T>(0.0), epsilon);
}

glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale);

glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::quat& orientation, const float scale);

glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::vec3& orientationInEulerAngles, const glm::vec3& scale);

glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::vec3& orientation, const float scale);

glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::vec3& orientation);

glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::quat& orientation);

float BarryCentric(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec2& pos);

glm::vec3 GetUpVector(const glm::quat& q);

glm::vec3 GetRightVector(const glm::quat& q);

glm::vec3 GetForwardVector(const glm::quat& q);

glm::vec3 GetRightVector(const glm::mat4& viewMatrix);

glm::vec3 GetUpVector(const glm::mat4& viewMatrix);

glm::vec3 GetForwardVector(const glm::mat4& viewMatrix);

bool DecomposeTransform(const glm::mat4& transform, glm::quat& rotation, glm::vec3& translation, glm::vec3& scale);

glm::vec3 ExtractScale(const glm::mat4& transform);

glm::vec3 ExtractTranslation(const glm::mat4& transform);

glm::quat ExtractRotationAsQuaternion(const glm::mat4& transform);

glm::mat4 ExtractRotation(const glm::mat4& transform);

std::vector<glm::vec3> GetFrustumCorners(const glm::mat4& inverseWorldToClipSpaceTransform);

glm::vec2 FromViewPortSpaceToNormalizedDeviceSpace(const glm::vec2& viewPortDimensions, const glm::vec2& viewPortCoords);

glm::vec4 FromNormalizedDeviceSpaceToClipSpace(const glm::vec2& normalizedDeviceSpaceCorrds);

glm::vec4 FromClipSpaceToCameraSpace(const glm::mat4& projectionMatrix, const glm::vec4& clipSpaceCoords);

glm::vec3 FromCameraSpaceToWorldSpace(const glm::mat4& viewMatrix, const glm::vec4& cameraSpaceCoords);

glm::mat4 CreatePerspectiveProjectionMatrix(const float aspectRatio, const float verticalFovInDegs, const float nearClippingPlane, const float farClippingPlane);

glm::mat4 CreatePerspectiveProjectionMatrixWithReverseDepth(const float aspectRatio, const float verticalFovInDegs, const float nearClippingPlane, const float farClippingPlane);

glm::mat4 CreateOrthographicProjectionMatrix(const float leftPlane, const float rightPlane, const float bottomPlane, const float topPlane, const float nearPlane, const float farPlane);

} // namespace prev::util::math

#endif