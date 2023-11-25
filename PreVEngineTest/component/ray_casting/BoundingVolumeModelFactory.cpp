#include "BoundingVolumeModelFactory.h"

#include "../../render/model/Model.h"

#include <prev/core/AllocatorProvider.h>

namespace prev_test::component::ray_casting {
namespace {
    std::tuple<std::vector<glm::vec3>, std::vector<uint32_t>> GetBoxData(const std::vector<glm::vec3>& boxPoints)
    {
        const std::vector<glm::vec3> vertices = {
            // front
            boxPoints[2],
            boxPoints[6],
            boxPoints[7],
            boxPoints[3],
            // back
            boxPoints[0],
            boxPoints[4],
            boxPoints[5],
            boxPoints[1],
            // top
            boxPoints[3],
            boxPoints[7],
            boxPoints[5],
            boxPoints[1],
            // bottom
            boxPoints[2],
            boxPoints[6],
            boxPoints[4],
            boxPoints[0],
            // left
            boxPoints[2],
            boxPoints[3],
            boxPoints[1],
            boxPoints[0],
            // rightt
            boxPoints[1],
            boxPoints[7],
            boxPoints[5],
            boxPoints[4]
        };

        const std::vector<uint32_t> indices = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4,
            8, 9, 10, 10, 11, 8,
            12, 13, 14, 14, 15, 12,
            16, 17, 18, 18, 19, 16,
            20, 21, 22, 22, 23, 20
        };

        return { vertices, indices };
    }

    std::tuple<std::vector<glm::vec3>, std::vector<uint32_t>> GenerateSphereData(const prev_test::common::intersection::Sphere& sphere)
    {
        const float degreesHorizontal{ 360.0f };
        const float degreesVertical{ 180.0f };
        const int subDivY{ 16 };
        const int subDivZ{ 16 };
        const float radius{ sphere.radius };
        const glm::vec3 positionOffset{ sphere.position };

        std::vector<glm::vec3> vertices;
        std::vector<uint32_t> indices;
        uint32_t indexBase{ 0 };

        float addAngleY{ -degreesHorizontal / static_cast<float>(subDivY) };
        float addAngleZ{ degreesVertical / static_cast<float>(subDivZ) };
        float curAngleY{ 0.0f };
        int stepsY{ 1 };

        while (stepsY <= subDivY) {
            const float sinY{ sinf(glm::radians(curAngleY)) };
            const float cosY{ cosf(glm::radians(curAngleY)) };
            const glm::vec3 directionY{ cosY, 0.0f, -sinY };

            const float nextAngleY{ curAngleY + addAngleY };
            const float nextSinY{ sinf(glm::radians(nextAngleY)) };
            const float nextCosY{ cosf(glm::radians(nextAngleY)) };
            const glm::vec3 nextDirectionY{ nextCosY, 0.0f, -nextSinY };

            float currentAngleZ{ 0.0f };
            int stepsZ{ 1 };
            while (stepsZ <= subDivZ) {
                const float sinZ{ sinf(glm::radians(currentAngleZ)) };
                const float cosZ{ cosf(glm::radians(currentAngleZ)) };

                const float nextAngleZ{ currentAngleZ + addAngleZ };
                const float nextSinZ{ sinf(glm::radians(nextAngleZ)) };
                const float nextCosZ{ cosf(glm::radians(nextAngleZ)) };

                const glm::vec3 quadPoints[] = {
                    { directionY.x * sinZ * radius, cosZ * radius, directionY.z * sinZ * radius },
                    { directionY.x * nextSinZ * radius, nextCosZ * radius, directionY.z * nextSinZ * radius },
                    { nextDirectionY.x * nextSinZ * radius, nextCosZ * radius, nextDirectionY.z * nextSinZ * radius },
                    { nextDirectionY.x * sinZ * radius, cosZ * radius, nextDirectionY.z * sinZ * radius }
                };

                for (const auto pt : quadPoints) {
                    vertices.push_back(positionOffset + pt);
                }

                const uint32_t quadIndices[] = { 0, 1, 2, 2, 3, 0 };
                for (const auto idx : quadIndices) {
                    indices.push_back(indexBase + idx);
                }
                indexBase += 6;

                stepsZ++;
                currentAngleZ += addAngleZ;
            }
            stepsY++;
            curAngleY += addAngleY;
        }
        return { vertices, indices };
    }
} // namespace

std::unique_ptr<prev_test::render::IModel> BoundingVolumeModelFactory::CreateAABBModel(const prev_test::common::intersection::AABB& aabb) const
{
    const auto aabbPoints{ aabb.GetPoints() };

    const auto [vertices, indices] = GetBoxData(aabbPoints);

    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    auto vertexBuffer{ std::make_unique<prev::render::buffer::HostVisibleVertexBuffer>(*allocator, static_cast<uint32_t>(vertices.size())) };
    vertexBuffer->Data(vertices.data(), static_cast<uint32_t>(vertices.size()), sizeof(glm::vec3));
    auto indexBuffer{ std::make_unique<prev::render::buffer::HostVisibleIndexBuffer>(*allocator, static_cast<uint32_t>(indices.size())) };
    indexBuffer->Data(indices.data(), static_cast<uint32_t>(indices.size()));

    return std::make_unique<prev_test::render::model::Model>(nullptr, std::move(vertexBuffer), std::move(indexBuffer));
}

std::unique_ptr<prev_test::render::IModel> BoundingVolumeModelFactory::CreateAABBModel(const prev_test::common::intersection::AABB& aabb, const std::shared_ptr<prev_test::render::IModel>& model) const
{
    const auto aabbPoints{ aabb.GetPoints() };

    const auto [vertices, indices] = GetBoxData(aabbPoints);

    const auto& vertexBuffer{ model->GetVertexBuffer() };
    const auto& indexBuffer{ model->GetIndexBuffer() };
    vertexBuffer->Data(vertices.data(), static_cast<uint32_t>(vertices.size()), sizeof(glm::vec3));
    indexBuffer->Data(indices.data(), static_cast<uint32_t>(indices.size()));

    return std::make_unique<prev_test::render::model::Model>(nullptr, std::move(vertexBuffer), std::move(indexBuffer));
}

std::unique_ptr<prev_test::render::IModel> BoundingVolumeModelFactory::CreateOBBModel(const prev_test::common::intersection::OBB& obb) const
{
    const auto obbPoints{ obb.GetPoints() };

    const auto [vertices, indices] = GetBoxData(obbPoints);

    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    auto vertexBuffer{ std::make_unique<prev::render::buffer::HostVisibleVertexBuffer>(*allocator, static_cast<uint32_t>(vertices.size())) };
    vertexBuffer->Data(vertices.data(), static_cast<uint32_t>(vertices.size()), sizeof(glm::vec3));
    auto indexBuffer{ std::make_unique<prev::render::buffer::HostVisibleIndexBuffer>(*allocator, static_cast<uint32_t>(indices.size())) };
    indexBuffer->Data(indices.data(), static_cast<uint32_t>(indices.size()));

    return std::make_unique<prev_test::render::model::Model>(nullptr, std::move(vertexBuffer), std::move(indexBuffer));
}

std::unique_ptr<prev_test::render::IModel> BoundingVolumeModelFactory::CreateOBBModel(const prev_test::common::intersection::OBB& obb, const std::shared_ptr<prev_test::render::IModel>& model) const
{
    const auto obbPoints{ obb.GetPoints() };

    const auto [vertices, indices] = GetBoxData(obbPoints);

    const auto& vertexBuffer{ model->GetVertexBuffer() };
    const auto& indexBuffer{ model->GetIndexBuffer() };
    vertexBuffer->Data(vertices.data(), static_cast<uint32_t>(vertices.size()), sizeof(glm::vec3));
    indexBuffer->Data(indices.data(), static_cast<uint32_t>(indices.size()));

    return std::make_unique<prev_test::render::model::Model>(nullptr, std::move(vertexBuffer), std::move(indexBuffer));
}

std::unique_ptr<prev_test::render::IModel> BoundingVolumeModelFactory::CreateSphereModel(const prev_test::common::intersection::Sphere& sphere) const
{
    const auto [vertices, indices] = GenerateSphereData(sphere);

    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    auto vertexBuffer{ std::make_unique<prev::render::buffer::HostVisibleVertexBuffer>(*allocator, static_cast<uint32_t>(vertices.size())) };
    vertexBuffer->Data(vertices.data(), static_cast<uint32_t>(vertices.size()), sizeof(glm::vec3));
    auto indexBuffer{ std::make_unique<prev::render::buffer::HostVisibleIndexBuffer>(*allocator, static_cast<uint32_t>(indices.size())) };
    indexBuffer->Data(indices.data(), static_cast<uint32_t>(indices.size()));

    return std::make_unique<prev_test::render::model::Model>(nullptr, std::move(vertexBuffer), std::move(indexBuffer));
}

std::unique_ptr<prev_test::render::IModel> BoundingVolumeModelFactory::CreateSphereModel(const prev_test::common::intersection::Sphere& sphere, const std::shared_ptr<prev_test::render::IModel>& model) const
{
    const auto [vertices, indices] = GenerateSphereData(sphere);

    const auto& vertexBuffer{ model->GetVertexBuffer() };
    const auto& indexBuffer{ model->GetIndexBuffer() };
    vertexBuffer->Data(vertices.data(), static_cast<uint32_t>(vertices.size()), sizeof(glm::vec3));
    indexBuffer->Data(indices.data(), static_cast<uint32_t>(indices.size()));

    return std::make_unique<prev_test::render::model::Model>(nullptr, std::move(vertexBuffer), std::move(indexBuffer));
}
} // namespace prev_test::component::ray_casting