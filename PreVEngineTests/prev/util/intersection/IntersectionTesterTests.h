#ifndef __INTERSECTION_TESTER_TESTS_H__
#define __INTERSECTION_TESTER_TESTS_H__

#include <prev/common/Common.h>
#include <prev/util/MathUtils.h>
#include <prev/util/intersection/IntersectionTester.h>

#include <gtest/gtest.h>
#include <iostream>

namespace prev::util::intersection::tester {

TEST(IntersectionTesterTests, SpherePlaneIntersection_Positive)
{
    Sphere sphere{ glm::vec3(0.0f, 0.0f, 0.0f), 1.0f };
    Plane plane{ glm::vec3(0.0f, 1.0f, 0.0f), 0.0f };

    EXPECT_TRUE(Intersects(sphere, plane));
}

TEST(IntersectionTesterTests, SpherePlaneIntersection_Negative)
{
    Sphere sphere{ glm::vec3(0.0f, 0.0f, 0.0f), 1.0f };
    Plane plane{ glm::vec3(0.0f, 1.0f, 0.0f), 2.0f };

    EXPECT_FALSE(Intersects(sphere, plane));
}

TEST(IntersectionTesterTests, AABBPlaneIntersection_Positive)
{
    AABB box{ glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f) };
    Plane plane{ glm::vec3(0.0f, 1.0f, 0.0f), 0.0f };

    EXPECT_TRUE(Intersects(box, plane));
}

TEST(IntersectionTesterTests, AABBPlaneIntersection_Negative)
{
    AABB box{ glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f) };
    Plane plane{ glm::vec3(0.0f, 1.0f, 0.0f), 5.0f };

    EXPECT_FALSE(Intersects(box, plane));
}

TEST(IntersectionTesterTests, PlanePointIntersection_Positive)
{
    Plane plane{ glm::vec3(0.0f, 1.0f, 0.0f), 0.0f };
    Point point{ glm::vec3(0.0f, 0.0f, 0.0f) };

    EXPECT_TRUE(Intersects(plane, point));
}

TEST(IntersectionTesterTests, PlanePointIntersection_Negative)
{
    Plane plane{ glm::vec3(0.0f, 1.0f, 0.0f), 0.0f };
    Point point{ glm::vec3(0.0f, 5.0f, 0.0f) };

    EXPECT_FALSE(Intersects(plane, point));
}

TEST(IntersectionTesterTests, AABBPointIntersection_Positive)
{
    AABB box{ glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f) };
    Point point{ glm::vec3(0.0f, 0.0f, 0.0f) };

    EXPECT_TRUE(Intersects(box, point));
}

TEST(IntersectionTesterTests, AABBPointIntersection_Negative)
{
    AABB box{ glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f) };
    Point point{ glm::vec3(5.0f, 5.0f, 5.0f) };

    EXPECT_FALSE(Intersects(box, point));
}

TEST(IntersectionTesterTests, SpherePointIntersection_Positive)
{
    Sphere sphere{ glm::vec3(0.0f, 0.0f, 0.0f), 1.0f };
    Point point{ glm::vec3(0.0f, 0.0f, 0.0f) };

    EXPECT_TRUE(Intersects(sphere, point));
}

TEST(IntersectionTesterTests, SpherePointIntersection_Negative)
{
    Sphere sphere{ glm::vec3(0.0f, 0.0f, 0.0f), 1.0f };
    Point point{ glm::vec3(5.0f, 5.0f, 5.0f) };

    EXPECT_FALSE(Intersects(sphere, point));
}

TEST(IntersectionTesterTests, AABBAABBIntersection)
{
    AABB box1{ glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f) };
    AABB box2{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f, 2.0f, 2.0f) };

    EXPECT_TRUE(Intersects(box1, box2));
}

TEST(IntersectionTesterTests, AABBAABBIntersection_Negative)
{
    AABB box1{ glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f) };
    AABB box2{ glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(6.0f, 6.0f, 6.0f) };

    EXPECT_FALSE(Intersects(box1, box2));
}

TEST(IntersectionTesterTests, SphereSphereIntersection)
{
    Sphere sphere1{ glm::vec3(0.0f, 0.0f, 0.0f), 1.0f };
    Sphere sphere2{ glm::vec3(1.0f, 0.0f, 0.0f), 1.0f };

    EXPECT_TRUE(Intersects(sphere1, sphere2));
}

TEST(IntersectionTesterTests, SphereSphereIntersection_Negative)
{
    Sphere sphere1{ glm::vec3(0.0f, 0.0f, 0.0f), 1.0f };
    Sphere sphere2{ glm::vec3(5.0f, 5.0f, 5.0f), 1.0f };

    EXPECT_FALSE(Intersects(sphere1, sphere2));
}

TEST(IntersectionTesterTests, SphereAABBIntersection)
{
    Sphere sphere{ glm::vec3(0.0f, 0.0f, 0.0f), 1.0f };
    AABB box{ glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f) };

    EXPECT_TRUE(Intersects(sphere, box));
}

TEST(IntersectionTesterTests, SphereAABBIntersection_Negative)
{
    Sphere sphere{ glm::vec3(5.0f, 5.0f, 5.0f), 1.0f };
    AABB box{ glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f) };

    EXPECT_FALSE(Intersects(sphere, box));
}

TEST(IntersectionTesterTests, FrustumPointIntersection_Positive)
{
    glm::mat4 projectionMatrix = prev::util::math::CreatePerspectiveProjectionMatrix(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
    glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Frustum frustum{ projectionMatrix, viewMatrix };
    Point point{ glm::vec3(0.5f, 0.5f, 0.5f) };

    EXPECT_TRUE(Intersects(frustum, point));
}

TEST(IntersectionTesterTests, FrustumPointIntersection_Negative)
{
    glm::mat4 projectionMatrix = prev::util::math::CreatePerspectiveProjectionMatrix(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
    glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Frustum frustum{ projectionMatrix, viewMatrix };
    Point point{ glm::vec3(5.0f, 5.0f, 5.0f) };

    EXPECT_FALSE(Intersects(frustum, point));
}

TEST(IntersectionTesterTests, FrustumSphereIntersection_Positive)
{
    glm::mat4 projectionMatrix = prev::util::math::CreatePerspectiveProjectionMatrix(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
    glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Frustum frustum{ projectionMatrix, viewMatrix };
    Sphere sphere{ glm::vec3(0.0f, 0.0f, 0.0f), 0.5f };

    EXPECT_TRUE(Intersects(frustum, sphere));
}

TEST(IntersectionTesterTests, FrustumSphereIntersection_Negative)
{
    glm::mat4 projectionMatrix = prev::util::math::CreatePerspectiveProjectionMatrix(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
    glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Frustum frustum{ projectionMatrix, viewMatrix };
    Sphere sphere{ glm::vec3(5.0f, 5.0f, -5.0f), 0.5f };

    EXPECT_FALSE(Intersects(frustum, sphere));
}

TEST(IntersectionTesterTests, FrustumAABBIntersection_Positive)
{
    glm::mat4 projectionMatrix = prev::util::math::CreatePerspectiveProjectionMatrix(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
    glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Frustum frustum{ projectionMatrix, viewMatrix };
    AABB box{ glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f) };

    EXPECT_TRUE(Intersects(frustum, box));
}

TEST(IntersectionTesterTests, FrustumAABBIntersection_Positive2)
{
    glm::mat4 projectionMatrix = prev::util::math::CreatePerspectiveProjectionMatrix(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
    glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Frustum frustum{ projectionMatrix, viewMatrix };
    AABB box{ glm::vec3(-10.0f, -10.0f, -10.0f), glm::vec3(10.0f, 10.0f, 10.0f) };

    EXPECT_TRUE(Intersects(frustum, box));
}

TEST(IntersectionTesterTests, FrustumAABBIntersection_Positive3)
{
    glm::mat4 projectionMatrix = prev::util::math::CreatePerspectiveProjectionMatrix(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
    glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Frustum frustum{ projectionMatrix, viewMatrix };
    AABB box{ glm::vec3(-100.0f, -100.0f, -100.0f), glm::vec3(100.0f, 100.0f, 100.0f) };

    EXPECT_TRUE(Intersects(frustum, box));
}

TEST(IntersectionTesterTests, FrustumAABBIntersection_Negative)
{
    glm::mat4 projectionMatrix = prev::util::math::CreatePerspectiveProjectionMatrix(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
    glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Frustum frustum{ projectionMatrix, viewMatrix };
    AABB box{ glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(6.0f, 6.0f, 6.0f) };

    EXPECT_FALSE(Intersects(frustum, box));
}

TEST(IntersectionTesterTests, FrustumOBBIntersection_Positive)
{
    glm::mat4 projectionMatrix = prev::util::math::CreatePerspectiveProjectionMatrix(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
    glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Frustum frustum{ projectionMatrix, viewMatrix };
    OBB obb{ glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f) };

    EXPECT_TRUE(Intersects(obb, frustum));
}

TEST(IntersectionTesterTests, FrustumOBBIntersection_Negative)
{
    glm::mat4 projectionMatrix = prev::util::math::CreatePerspectiveProjectionMatrix(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
    glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Frustum frustum{ projectionMatrix, viewMatrix };
    OBB obb{ glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)), glm::vec3(5.5f, 5.5f, 5.5f), glm::vec3(1.0f) };

    EXPECT_FALSE(Intersects(obb, frustum));
}

TEST(IntersectionTesterTests, RayAABBIntersection_Positive)
{
    Ray ray{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f };
    AABB box{ glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(2.0f, 1.0f, 1.0f) };
    RayCastResult result;

    EXPECT_TRUE(Intersects(ray, box, result));
}

TEST(IntersectionTesterTests, RayAABBIntersection_Negative)
{
    Ray ray{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f };
    AABB box{ glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(6.0f, 6.0f, 6.0f) };
    RayCastResult result;

    EXPECT_FALSE(Intersects(ray, box, result));
}

TEST(IntersectionTesterTests, RaySphereIntersection_Positive)
{
    Ray ray{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f };
    Sphere sphere{ glm::vec3(1.0f, 0.0f, 0.0f), 0.5f };
    RayCastResult result;

    EXPECT_TRUE(Intersects(ray, sphere, result));
}

TEST(IntersectionTesterTests, RaySphereIntersection_Negative)
{
    Ray ray{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f };
    Sphere sphere{ glm::vec3(5.0f, 5.0f, 5.0f), 0.5f };
    RayCastResult result;

    EXPECT_FALSE(Intersects(ray, sphere, result));
}

TEST(IntersectionTesterTests, RayPlaneIntersection_Positive)
{
    Ray ray{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 2.0f };
    Plane plane{ glm::vec3(0.0f, 1.0f, 0.0f), 1.0f };
    RayCastResult result;

    EXPECT_TRUE(Intersects(ray, plane, result));
}

TEST(IntersectionTesterTests, RayPlaneIntersection_Negative)
{
    Ray ray{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f };
    Plane plane{ glm::vec3(0.0f, 1.0f, 0.0f), -5.0f };
    RayCastResult result;

    EXPECT_FALSE(Intersects(ray, plane, result));
}

TEST(IntersectionTesterTests, RayOBBIntersection_Positive)
{
    Ray ray{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f };
    OBB obb{ glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f) };
    RayCastResult result;

    EXPECT_TRUE(Intersects(ray, obb, result));
}

TEST(IntersectionTesterTests, RayOBBIntersection_Negative)
{
    Ray ray{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f };
    OBB obb{ glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)), glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(1.0f) };
    RayCastResult result;

    EXPECT_FALSE(Intersects(ray, obb, result));
}

} // namespace prev::util::intersection::tester

#endif // !__INTERSECTION_TESTER_TESTS_H__