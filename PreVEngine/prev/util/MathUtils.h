#ifndef __MATH_UTILS_H__
#define __MATH_UTILS_H__

#include "../common/Common.h"

#include <vector>

namespace prev::util {
class MathUtil {
public:
    static glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale)
    {
        glm::mat4 resultTransform(1.0f);
        resultTransform = glm::translate(resultTransform, position);
        resultTransform *= glm::mat4_cast(glm::normalize(orientation));
        resultTransform = glm::scale(resultTransform, scale);
        return resultTransform;
    }

    static glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::quat& orientation, const float scale)
    {
        return MathUtil::CreateTransformationMatrix(position, orientation, glm::vec3(scale));
    }

    static glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::vec3& orientationInEulerAngles, const glm::vec3& scale)
    {
        glm::quat orientation = glm::normalize(glm::quat(glm::vec3(glm::radians(orientationInEulerAngles))));
        return MathUtil::CreateTransformationMatrix(position, orientation, scale);
    }

    static glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::vec3& orientation, const float scale)
    {
        return MathUtil::CreateTransformationMatrix(position, orientation, glm::vec3(scale));
    }

    static glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::vec3& orientation)
    {
        return MathUtil::CreateTransformationMatrix(position, orientation, glm::vec3(1.0f));
    }

    static glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::quat& orientation)
    {
        return MathUtil::CreateTransformationMatrix(position, orientation, glm::vec3(1.0f));
    }

    static float BarryCentric(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec2& pos)
    {
        const float det{ (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z) };
        const float l1{ ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det };
        const float l2{ ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det };
        const float l3{ 1.0f - l1 - l2 };
        return l1 * p1.y + l2 * p2.y + l3 * p3.y;
    }

    static glm::vec3 GetUpVector(const glm::quat& q)
    {
        return glm::normalize(q * glm::vec3(0.0f, 1.0f, 0.0f));
    }

    static glm::vec3 GetRightVector(const glm::quat& q)
    {
        return glm::normalize(q * glm::vec3(1.0f, 0.0f, 0.0f));
    }

    static glm::vec3 GetForwardVector(const glm::quat& q)
    {
        return glm::normalize(q * glm::vec3(0.0f, 0.0f, 1.0f));
    }

    static glm::vec3 GetRightVector(const glm::mat4 viewMatrix)
    {
        return glm::normalize(glm::vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]));
    }

    static glm::vec3 GetUpVector(const glm::mat4 viewMatrix)
    {
        return glm::normalize(glm::vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]));
    }

    static glm::vec3 GetForwardVector(const glm::mat4 viewMatrix)
    {
        return glm::normalize(glm::vec3(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]));
    }

    template <typename Type>
    static int Clamp(Type val, Type min, Type max)
    {
        return (val < min ? min : val > max ? max : val);
    }

    static bool DecomposeTransform(const glm::mat4& transform, glm::quat& rotation, glm::vec3& translation, glm::vec3& scale)
    {
        glm::vec3 skew;
        glm::vec4 perspective;
        return glm::decompose(transform, scale, rotation, translation, skew, perspective);
    }

    static glm::vec3 ExtractScale(const glm::mat4& transform)
    {
        glm::quat rotation;
        glm::vec3 translation, scale;
        DecomposeTransform(transform, rotation, translation, scale);
        return scale;
    }

    static glm::vec3 ExtractTranslation(const glm::mat4& transform)
    {
        glm::quat rotation;
        glm::vec3 translation, scale;
        DecomposeTransform(transform, rotation, translation, scale);
        return translation;
    }

    static glm::mat4 ExtractRotation(const glm::mat4& transform)
    {
        const auto rotAsQuat{ ExtractRotationAsQuaternion(transform) };
        return glm::mat4_cast(rotAsQuat);
    }

    static glm::quat ExtractRotationAsQuaternion(const glm::mat4& transform)
    {
        glm::quat rotation;
        glm::vec3 translation, scale;
        DecomposeTransform(transform, rotation, translation, scale);
        return rotation;
    }

    static uint32_t Log2(const uint32_t x)
    {
        return static_cast<uint32_t>(log(x) / log(2));
    }

    static uint32_t RoundUp(const uint32_t val, const uint32_t toDivBy)
    {
        return val + (toDivBy - (val % toDivBy));
    }

    static uint32_t RoundDown(const uint32_t val, const uint32_t toDivBy)
    {
        return val - (val % toDivBy);
    }

    static std::vector<glm::vec3> GetFrustumCorners(const glm::mat4& inverseWorldToClipSpaceTransform)
    {
        std::vector<glm::vec3> frustumCorners{
            { -1.0f, 1.0f, -1.0f },
            { 1.0f, 1.0f, -1.0f },
            { 1.0f, -1.0f, -1.0f },
            { -1.0f, -1.0f, -1.0f },
            { -1.0f, 1.0f, 1.0f },
            { 1.0f, 1.0f, 1.0f },
            { 1.0f, -1.0f, 1.0f },
            { -1.0f, -1.0f, 1.0f }
        };

        // Project frustum corners into world space(from clip space)
        for (uint32_t i = 0; i < 8; i++) {
            glm::vec4 invCorner = inverseWorldToClipSpaceTransform * glm::vec4(frustumCorners[i], 1.0f);
            frustumCorners[i] = invCorner / invCorner.w;
        }

        return frustumCorners;
    }

    static glm::vec2 FromViewPortSpaceToNormalizedDeviceSpace(const glm::vec2& viewPortDimensions, const glm::vec2& viewPortCoords)
    {
        const float x = (2.0f * viewPortCoords.x) / viewPortDimensions.x - 1.0f;
        const float y = (2.0f * viewPortCoords.y) / viewPortDimensions.y - 1.0f;
        return glm::vec2(x, -y);
    }

    static glm::vec4 FromNormalizedDeviceSpaceToClipSpace(const glm::vec2& normalizedDeviceSpaceCorrds)
    {
        return glm::vec4(normalizedDeviceSpaceCorrds.x, normalizedDeviceSpaceCorrds.y, -1.0f, 1.0f);
    }

    static glm::vec4 FromClipSpaceToCameraSpace(const glm::mat4& projectionMatrix, const glm::vec4& clipSpaceCoords)
    {
        const glm::mat4 invertedProjectionMatrix = glm::inverse(projectionMatrix);
        const glm::vec4 eyeCoords = invertedProjectionMatrix * clipSpaceCoords;
        return glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
    }

    static glm::vec3 FromCameraSpaceToWorldSpace(const glm::mat4& viewMatrix, const glm::vec4& cameraSpaceCoords)
    {
        const glm::mat4 invertedVieMatrix = glm::inverse(viewMatrix);
        const glm::vec4 worldCoords = invertedVieMatrix * cameraSpaceCoords;
        return glm::vec3(worldCoords.x, worldCoords.y, worldCoords.z);
    }
};
} // namespace prev::util

#endif