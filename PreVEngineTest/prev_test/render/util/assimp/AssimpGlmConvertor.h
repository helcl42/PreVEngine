#ifndef __ASSIMP_COMMON_H__
#define __ASSIMP_COMMON_H__

#include <prev/common/Common.h>

#include <assimp/scene.h>

namespace prev_test::render::util::assimp {
class AssimpGlmConvertor {
public:
    static glm::mat4 ToGlmMat4(const aiMatrix3x3& inputMatrix);

    static glm::mat4 ToGlmMat4(const aiMatrix4x4& inputMatrix);

    static glm::vec3 ToGlmVec3(const aiVector3D& inputVector);

    static glm::vec2 ToGlmVec2(const aiVector2D& inputVector);
};
} // namespace prev_test::render::util::assimp

#endif // !__ASSIMP_COMMON_H__
