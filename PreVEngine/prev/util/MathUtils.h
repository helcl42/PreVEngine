#ifndef __MATH_UTILS_H__
#define __MATH_UTILS_H__

#include "../common/Common.h"

#include <vector>

namespace prev::util::math {

template <typename T>
inline constexpr T Clamp(T val, T min, T max)
{
    return val < min ? min : val > max ? max : val;
}

template <typename T>
inline T Log2(const T x)
{
    return static_cast<T>(log(x) / log(static_cast<T>(2)));
}

template <typename T>
inline T RoundUp(const T val, const T toDivBy)
{
    return val + (toDivBy - (val % toDivBy));
}

template <typename T>
inline T RoundDown(const T val, const T toDivBy)
{
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

inline glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale)
{
    glm::mat4 resultTransform(1.0f);
    resultTransform = glm::translate(resultTransform, position);
    resultTransform *= glm::mat4_cast(glm::normalize(orientation));
    resultTransform = glm::scale(resultTransform, scale);
    return resultTransform;
}

inline glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::quat& orientation, const float scale)
{
    return CreateTransformationMatrix(position, orientation, glm::vec3(scale));
}

inline glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::vec3& orientationInEulerAngles, const glm::vec3& scale)
{
    const glm::quat orientation{ glm::normalize(glm::quat(glm::vec3(glm::radians(orientationInEulerAngles)))) };
    return CreateTransformationMatrix(position, orientation, scale);
}

inline glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::vec3& orientation, const float scale)
{
    return CreateTransformationMatrix(position, orientation, glm::vec3(scale));
}

inline glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::vec3& orientation)
{
    return CreateTransformationMatrix(position, orientation, glm::vec3(1.0f));
}

inline glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::quat& orientation)
{
    return CreateTransformationMatrix(position, orientation, glm::vec3(1.0f));
}

inline float BarryCentric(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec2& pos)
{
    const float det{ (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z) };
    const float l1{ ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det };
    const float l2{ ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det };
    const float l3{ 1.0f - l1 - l2 };
    return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}

inline glm::vec3 GetUpVector(const glm::quat& q)
{
    return glm::normalize(q * glm::vec3(0.0f, 1.0f, 0.0f));
}

inline glm::vec3 GetRightVector(const glm::quat& q)
{
    return glm::normalize(q * glm::vec3(1.0f, 0.0f, 0.0f));
}

inline glm::vec3 GetForwardVector(const glm::quat& q)
{
    return glm::normalize(q * glm::vec3(0.0f, 0.0f, 1.0f));
}

inline glm::vec3 GetRightVector(const glm::mat4 viewMatrix)
{
    return glm::normalize(glm::vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]));
}

inline glm::vec3 GetUpVector(const glm::mat4 viewMatrix)
{
    return glm::normalize(glm::vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]));
}

inline glm::vec3 GetForwardVector(const glm::mat4 viewMatrix)
{
    return glm::normalize(glm::vec3(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]));
}

inline bool DecomposeTransform(const glm::mat4& transform, glm::quat& rotation, glm::vec3& translation, glm::vec3& scale)
{
    glm::vec3 skew;
    glm::vec4 perspective;
    return glm::decompose(transform, scale, rotation, translation, skew, perspective);
}

inline glm::vec3 ExtractScale(const glm::mat4& transform)
{
    glm::quat rotation;
    glm::vec3 translation, scale;
    DecomposeTransform(transform, rotation, translation, scale);
    return scale;
}

inline glm::vec3 ExtractTranslation(const glm::mat4& transform)
{
    glm::quat rotation;
    glm::vec3 translation, scale;
    DecomposeTransform(transform, rotation, translation, scale);
    return translation;
}

inline glm::quat ExtractRotationAsQuaternion(const glm::mat4& transform)
{
    glm::quat rotation;
    glm::vec3 translation, scale;
    DecomposeTransform(transform, rotation, translation, scale);
    return rotation;
}

inline glm::mat4 ExtractRotation(const glm::mat4& transform)
{
    const auto rotAsQuat{ ExtractRotationAsQuaternion(transform) };
    return glm::mat4_cast(rotAsQuat);
}

inline std::vector<glm::vec3> GetFrustumCorners(const glm::mat4& inverseWorldToClipSpaceTransform)
{
    const std::vector<glm::vec3> cubeFrustumCorners{
        { -1.0f, 1.0f, 0.0f },
        { 1.0f, 1.0f, 0.0f },
        { 1.0f, -1.0f, 0.0f },
        { -1.0f, -1.0f, 0.0f },
        { -1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, -1.0f, 1.0f },
        { -1.0f, -1.0f, 1.0f }
    };

    // Project frustum corners into world space(from clip space)
    std::vector<glm::vec3> result(cubeFrustumCorners.size());
    for (uint32_t i = 0; i < 8; i++) {
        const glm::vec4 invCorner{ inverseWorldToClipSpaceTransform * glm::vec4(cubeFrustumCorners[i], 1.0f) };
        result[i] = invCorner / invCorner.w;
    }
    return result;
}

inline glm::vec2 FromViewPortSpaceToNormalizedDeviceSpace(const glm::vec2& viewPortDimensions, const glm::vec2& viewPortCoords)
{
    const float x{ (2.0f * viewPortCoords.x) / viewPortDimensions.x - 1.0f };
    const float y{ (2.0f * viewPortCoords.y) / viewPortDimensions.y - 1.0f };
    return glm::vec2(x, -y);
}

inline glm::vec4 FromNormalizedDeviceSpaceToClipSpace(const glm::vec2& normalizedDeviceSpaceCorrds)
{
    return glm::vec4(normalizedDeviceSpaceCorrds.x, normalizedDeviceSpaceCorrds.y, -1.0f, 1.0f);
}

inline glm::vec4 FromClipSpaceToCameraSpace(const glm::mat4& projectionMatrix, const glm::vec4& clipSpaceCoords)
{
    const glm::mat4 invertedProjectionMatrix{ glm::inverse(projectionMatrix) };
    const glm::vec4 eyeCoords{ invertedProjectionMatrix * clipSpaceCoords };
    return glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
}

inline glm::vec3 FromCameraSpaceToWorldSpace(const glm::mat4& viewMatrix, const glm::vec4& cameraSpaceCoords)
{
    const glm::mat4 invertedVieMatrix{ glm::inverse(viewMatrix) };
    const glm::vec4 worldCoords{ invertedVieMatrix * cameraSpaceCoords };
    return glm::vec3(worldCoords.x, worldCoords.y, worldCoords.z);
}
} // namespace prev::util::math

#endif