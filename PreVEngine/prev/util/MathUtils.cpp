#include "MathUtils.h"

namespace prev::util::math {
glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale)
{
    glm::mat4 resultTransform(1.0f);
    resultTransform = glm::translate(resultTransform, position);
    resultTransform *= glm::mat4_cast(glm::normalize(orientation));
    resultTransform = glm::scale(resultTransform, scale);
    return resultTransform;
}

glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::quat& orientation, const float scale)
{
    return CreateTransformationMatrix(position, orientation, glm::vec3(scale));
}

glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::vec3& orientationInEulerAngles, const glm::vec3& scale)
{
    const glm::quat orientation{ glm::normalize(glm::quat(glm::vec3(glm::radians(orientationInEulerAngles)))) };
    return CreateTransformationMatrix(position, orientation, scale);
}

glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::vec3& orientation, const float scale)
{
    return CreateTransformationMatrix(position, orientation, glm::vec3(scale));
}

glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::vec3& orientation)
{
    return CreateTransformationMatrix(position, orientation, glm::vec3(1.0f));
}

glm::mat4 CreateTransformationMatrix(const glm::vec3& position, const glm::quat& orientation)
{
    return CreateTransformationMatrix(position, orientation, glm::vec3(1.0f));
}

float BarryCentric(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec2& pos)
{
    const float det{ (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z) };
    const float l1{ ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det };
    const float l2{ ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det };
    const float l3{ 1.0f - l1 - l2 };
    return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}

glm::vec3 GetUpVector(const glm::quat& q)
{
    return glm::normalize(q * glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 GetRightVector(const glm::quat& q)
{
    return glm::normalize(q * glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::vec3 GetForwardVector(const glm::quat& q)
{
    return glm::normalize(q * glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::vec3 GetRightVector(const glm::mat4& viewMatrix)
{
    return glm::normalize(glm::vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]));
}

glm::vec3 GetUpVector(const glm::mat4& viewMatrix)
{
    return glm::normalize(glm::vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]));
}

glm::vec3 GetForwardVector(const glm::mat4& viewMatrix)
{
    return glm::normalize(glm::vec3(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]));
}

bool DecomposeTransform(const glm::mat4& transform, glm::quat& rotation, glm::vec3& translation, glm::vec3& scale)
{
    glm::vec3 skew;
    glm::vec4 perspective;
    return glm::decompose(transform, scale, rotation, translation, skew, perspective);
}

glm::vec3 ExtractScale(const glm::mat4& transform)
{
    glm::quat rotation;
    glm::vec3 translation, scale;
    DecomposeTransform(transform, rotation, translation, scale);
    return scale;
}

glm::vec3 ExtractTranslation(const glm::mat4& transform)
{
    glm::quat rotation;
    glm::vec3 translation, scale;
    DecomposeTransform(transform, rotation, translation, scale);
    return translation;
}

glm::quat ExtractRotationAsQuaternion(const glm::mat4& transform)
{
    glm::quat rotation;
    glm::vec3 translation, scale;
    DecomposeTransform(transform, rotation, translation, scale);
    return rotation;
}

glm::mat4 ExtractRotation(const glm::mat4& transform)
{
    const auto rotAsQuat{ ExtractRotationAsQuaternion(transform) };
    return glm::mat4_cast(rotAsQuat);
}

glm::quat CreateQuaternion(const glm::vec3& v1, const glm::vec3& v2)
{
    return glm::normalize(glm::quat(1.0f + glm::dot(v1, v2), glm::cross(v1, v2)));
}

std::vector<glm::vec3> GetFrustumCorners(const glm::mat4& inverseViewProjectionTransform)
{
    const std::vector<glm::vec3> frustumCornersNdc = {
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
    std::vector<glm::vec3> result(frustumCornersNdc.size());
    for (size_t i = 0; i < frustumCornersNdc.size(); ++i) {
        const glm::vec4 worldCorner{ inverseViewProjectionTransform * glm::vec4(frustumCornersNdc[i], 1.0f) };
        result[i] = worldCorner / worldCorner.w;
    }
    return result;
}

glm::vec2 FromViewPortSpaceToNormalizedDeviceSpace(const glm::vec2& viewPortDimensions, const glm::vec2& viewPortCoords)
{
    const float x{ (2.0f * viewPortCoords.x) / viewPortDimensions.x - 1.0f };
    const float y{ (2.0f * viewPortCoords.y) / viewPortDimensions.y - 1.0f };
    return glm::vec2(x, -y);
}

glm::vec4 FromNormalizedDeviceSpaceToClipSpace(const glm::vec2& normalizedDeviceSpaceCorrds)
{
    return glm::vec4(normalizedDeviceSpaceCorrds.x, normalizedDeviceSpaceCorrds.y, -1.0f, 1.0f);
}

glm::vec4 FromClipSpaceToCameraSpace(const glm::mat4& projectionMatrix, const glm::vec4& clipSpaceCoords)
{
    const glm::mat4 invertedProjectionMatrix{ glm::inverse(projectionMatrix) };
    const glm::vec4 eyeCoords{ invertedProjectionMatrix * clipSpaceCoords };
    return glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
}

glm::vec3 FromCameraSpaceToWorldSpace(const glm::mat4& viewMatrix, const glm::vec4& cameraSpaceCoords)
{
    const glm::mat4 invertedVieMatrix{ glm::inverse(viewMatrix) };
    const glm::vec4 worldCoords{ invertedVieMatrix * cameraSpaceCoords };
    return glm::vec3(worldCoords.x, worldCoords.y, worldCoords.z);
}

glm::mat4 CreateOrthographicProjectionMatrix(const float leftPlane, const float rightPlane, const float bottomPlane, const float topPlane, const float nearPlane, const float farPlane)
{
    const float x{ 2.0f / (rightPlane - leftPlane) };
    const float y{ -2.0f / (bottomPlane - topPlane) }; // invert Y in clip coordinates
    const float z{ 1.0f / (nearPlane - farPlane) };
    const float A{ -(rightPlane + leftPlane) / (rightPlane - leftPlane) };
    const float B{ -(bottomPlane + topPlane) / (bottomPlane - topPlane) };
    const float C{ nearPlane / (nearPlane - farPlane) };

    const glm::mat4 projectionMatrix = glm::mat4{
        x, 0.0f, 0.0f, 0.0f,
        0.0f, y, 0.0f, 0.0f,
        0.0f, 0.0f, z, 0.0f,
        A, B, C, 1.0f
    };
    return projectionMatrix;
}

glm::mat4 CreatePerspectiveProjectionMatrix(const float verticalFov, const float aspectRatio, const float nearClippingPlane, const float farClippingPlane)
{
    glm::mat4 projectionMatrix = glm::perspective(verticalFov, aspectRatio, nearClippingPlane, farClippingPlane);
    projectionMatrix[1][1] *= -1; // invert Y in clip coordinates
    return projectionMatrix;
}

glm::mat4 CreatePerspectiveProjectionMatrix(const float tanAngleLeft, const float tanAngleRight, const float tanAngleUp, float const tanAngleDown, const float nearClippingPlane, const float farClippingPlane)
{
    const float tanAngleWidth{ tanAngleRight - tanAngleLeft };
    const float tanAngleHeight{ tanAngleDown - tanAngleUp };

    const glm::mat4 projectionMatrix = glm::mat4{
        2.0f / tanAngleWidth, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / tanAngleHeight, 0.0f, 0.0f,
        (tanAngleRight + tanAngleLeft) / tanAngleWidth, (tanAngleUp + tanAngleDown) / tanAngleHeight, -farClippingPlane / (farClippingPlane - nearClippingPlane), -1.0f,
        0.0f, 0.0f, -(farClippingPlane * nearClippingPlane) / (farClippingPlane - nearClippingPlane), 0.0f
    };
    return projectionMatrix;
}

glm::mat4 CreatePerspectiveProjectionMatrix(const Fov& fov, const float nearClippingPlane, const float farClippingPlane)
{
    const float tanLeft{ std::tan(fov.angleLeft) };
    const float tanRight{ std::tan(fov.angleRight) };
    const float tanUp{ std::tan(fov.angleUp) };
    const float tanDown{ std::tan(fov.angleDown) };
    return CreatePerspectiveProjectionMatrix(tanLeft, tanRight, tanUp, tanDown, nearClippingPlane, farClippingPlane);
}

glm::vec2 GetClippingPlanes(const glm::mat4& projectionMatrix)
{
    const float nearPlane{ projectionMatrix[3][2] / projectionMatrix[2][2] };
    const float farPlane{ projectionMatrix[3][2] / (projectionMatrix[2][2] + 1.0f) };
    return { nearPlane, farPlane };
}

Fov CreateFovFromProjectionMatrix(const glm::mat4& projectionMatrix)
{
    const float left{ (projectionMatrix[2][0] - 1.0f) / projectionMatrix[0][0] };
    const float right{ (projectionMatrix[2][0] + 1.0f) / projectionMatrix[0][0] };
    const float up{ (projectionMatrix[2][1] - 1.0f) / projectionMatrix[1][1] };
    const float down{ (projectionMatrix[2][1] + 1.0f) / projectionMatrix[1][1] };

    return Fov{ std::atan(left), std::atan(right), std::atan(up), std::atan(down) };
}
} // namespace prev::util::math