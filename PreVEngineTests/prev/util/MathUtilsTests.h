#include <prev/common/Common.h>
#include <prev/util/MathUtils.h>

#include <gtest/gtest.h>

namespace prev::util::math {

TEST(MathUtilsTests, CreateTransformationMatrix_PositionOrientationScale)
{
    glm::vec3 position(1.0f, 2.0f, 3.0f);
    glm::quat orientation = glm::quat(glm::vec3(0.0f, glm::radians(90.0f), 0.0f));
    glm::vec3 scale(2.0f, 2.0f, 2.0f);

    glm::mat4 result = CreateTransformationMatrix(position, orientation, scale);

    glm::mat4 expected = glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(glm::normalize(orientation)) * glm::scale(glm::mat4(1.0f), scale);

    EXPECT_EQ(result, expected);
}

TEST(MathUtilsTests, CreateTransformationMatrix_PositionOrientationFloatScale)
{
    glm::vec3 position(1.0f, 2.0f, 3.0f);
    glm::quat orientation = glm::quat(glm::vec3(0.0f, glm::radians(90.0f), 0.0f));
    float scale = 2.0f;

    glm::mat4 result = CreateTransformationMatrix(position, orientation, scale);

    glm::mat4 expected = CreateTransformationMatrix(position, orientation, glm::vec3(scale));

    EXPECT_EQ(result, expected);
}

TEST(MathUtilsTests, CreateTransformationMatrix_PositionEulerAnglesScale)
{
    glm::vec3 position(1.0f, 2.0f, 3.0f);
    glm::vec3 eulerAngles(0.0f, 90.0f, 0.0f);
    glm::vec3 scale(2.0f, 2.0f, 2.0f);

    glm::mat4 result = CreateTransformationMatrix(position, eulerAngles, scale);

    glm::quat orientation = glm::normalize(glm::quat(glm::vec3(glm::radians(eulerAngles))));
    glm::mat4 expected = CreateTransformationMatrix(position, orientation, scale);

    EXPECT_EQ(result, expected);
}

TEST(MathUtilsTests, CreateTransformationMatrix_PositionEulerAnglesFloatScale)
{
    glm::vec3 position(1.0f, 2.0f, 3.0f);
    glm::vec3 eulerAngles(0.0f, 90.0f, 0.0f);
    float scale = 2.0f;

    glm::mat4 result = CreateTransformationMatrix(position, eulerAngles, scale);

    glm::mat4 expected = CreateTransformationMatrix(position, eulerAngles, glm::vec3(scale));

    EXPECT_EQ(result, expected);
}

TEST(MathUtilsTests, CreateTransformationMatrix_PositionEulerAngles)
{
    glm::vec3 position(1.0f, 2.0f, 3.0f);
    glm::vec3 eulerAngles(0.0f, 90.0f, 0.0f);

    glm::mat4 result = CreateTransformationMatrix(position, eulerAngles);

    glm::mat4 expected = CreateTransformationMatrix(position, eulerAngles, glm::vec3(1.0f));

    EXPECT_EQ(result, expected);
}

TEST(MathUtilsTests, CreateTransformationMatrix_PositionOrientation)
{
    glm::vec3 position(1.0f, 2.0f, 3.0f);
    glm::quat orientation = glm::quat(glm::vec3(0.0f, glm::radians(90.0f), 0.0f));

    glm::mat4 result = CreateTransformationMatrix(position, orientation);

    glm::mat4 expected = CreateTransformationMatrix(position, orientation, glm::vec3(1.0f));

    EXPECT_EQ(result, expected);
}

TEST(MathUtilsTests, BarryCentric)
{
    glm::vec3 p1(0.0f, 1.0f, 0.0f);
    glm::vec3 p2(1.0f, 2.0f, 0.0f);
    glm::vec3 p3(0.0f, 3.0f, 1.0f);
    glm::vec2 pos(0.5f, 0.5f);

    float result = BarryCentric(p1, p2, p3, pos);

    float expected = 2.5f; // Precomputed expected value

    EXPECT_FLOAT_EQ(result, expected);
}

TEST(MathUtilsTests, GetUpVector)
{
    glm::quat orientation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));

    glm::vec3 result = GetUpVector(orientation);

    glm::vec3 expected(0.0f, 1.0f, 0.0f);

    EXPECT_EQ(result, expected);
}

TEST(MathUtilsTests, GetRightVector)
{
    glm::quat orientation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));

    glm::vec3 result = GetRightVector(orientation);

    glm::vec3 expected(1.0f, 0.0f, 0.0f);

    EXPECT_EQ(result, expected);
}

TEST(MathUtilsTests, GetForwardVector)
{
    glm::quat orientation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));

    glm::vec3 result = GetForwardVector(orientation);

    glm::vec3 expected(0.0f, 0.0f, -1.0f);

    EXPECT_EQ(result, expected);
}

TEST(MathUtilsTests, CreatePerspectiveProjectionMatrix)
{
    float aspectRatio = 16.0f / 9.0f;
    float verticalFov = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;

    glm::mat4 result = CreatePerspectiveProjectionMatrix(aspectRatio, verticalFov, nearPlane, farPlane);

    glm::mat4 expected = glm::perspective(glm::radians(verticalFov), aspectRatio, nearPlane, farPlane);
    expected[1][1] *= -1; // Adjust for inverted Y

    EXPECT_EQ(result, expected);
}

} // namespace prev::util::math



