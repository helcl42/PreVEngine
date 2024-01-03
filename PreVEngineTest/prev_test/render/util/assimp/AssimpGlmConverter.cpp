#include "AssimpGlmConvertor.h"

namespace prev_test::render::util::assimp {
glm::mat4 AssimpGlmConvertor::ToGlmMat4(const aiMatrix3x3& inputMatrix)
{
    glm::mat4 newMatrix;
    newMatrix[0][0] = inputMatrix.a1;
    newMatrix[1][0] = inputMatrix.a2;
    newMatrix[2][0] = inputMatrix.a3;
    newMatrix[3][0] = 0.0f;
    newMatrix[0][1] = inputMatrix.b1;
    newMatrix[1][1] = inputMatrix.b2;
    newMatrix[2][1] = inputMatrix.b3;
    newMatrix[3][1] = 0.0f;
    newMatrix[0][2] = inputMatrix.c1;
    newMatrix[1][2] = inputMatrix.c2;
    newMatrix[2][2] = inputMatrix.c3;
    newMatrix[3][2] = 0.0f;
    newMatrix[0][3] = 0.0f;
    newMatrix[1][3] = 0.0f;
    newMatrix[2][3] = 0.0f;
    newMatrix[3][3] = 1.0f;
    return newMatrix;
}

glm::mat4 AssimpGlmConvertor::ToGlmMat4(const aiMatrix4x4& inputMatrix)
{
    glm::mat4 newMatrix;
    newMatrix[0][0] = inputMatrix.a1;
    newMatrix[1][0] = inputMatrix.a2;
    newMatrix[2][0] = inputMatrix.a3;
    newMatrix[3][0] = inputMatrix.a4;
    newMatrix[0][1] = inputMatrix.b1;
    newMatrix[1][1] = inputMatrix.b2;
    newMatrix[2][1] = inputMatrix.b3;
    newMatrix[3][1] = inputMatrix.b4;
    newMatrix[0][2] = inputMatrix.c1;
    newMatrix[1][2] = inputMatrix.c2;
    newMatrix[2][2] = inputMatrix.c3;
    newMatrix[3][2] = inputMatrix.c4;
    newMatrix[0][3] = inputMatrix.d1;
    newMatrix[1][3] = inputMatrix.d2;
    newMatrix[2][3] = inputMatrix.d3;
    newMatrix[3][3] = inputMatrix.d4;
    return newMatrix;
}

glm::vec3 AssimpGlmConvertor::ToGlmVec3(const aiVector3D& inputVector)
{
    return glm::vec3(inputVector.x, inputVector.y, inputVector.z);
}

glm::vec2 AssimpGlmConvertor::ToGlmVec2(const aiVector2D& inputVector)
{
    return glm::vec2(inputVector.x, inputVector.y);
}
} // namespace prev_test::render::util::assimp