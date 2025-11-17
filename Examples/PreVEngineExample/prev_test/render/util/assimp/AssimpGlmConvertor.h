#ifndef __ASSIMP_COMMON_H__
#define __ASSIMP_COMMON_H__

#include <prev/common/Common.h>

#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <assimp/vector2.h>
#include <assimp/vector3.h>

namespace prev_test::render::util::assimp {
glm::mat4 ToGlmMat4(const aiMatrix4x4& inputMatrix);

glm::vec3 ToGlmVec3(const aiVector3D& inputVector);

glm::vec2 ToGlmVec2(const aiVector2D& inputVector);

glm::quat ToGlmQuat(const aiQuaternion& inputQuaternion);
} // namespace prev_test::render::util::assimp

#endif // !__ASSIMP_COMMON_H__
