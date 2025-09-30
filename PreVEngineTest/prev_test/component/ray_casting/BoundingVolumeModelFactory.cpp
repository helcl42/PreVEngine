#include "BoundingVolumeModelFactory.h"

#include "../../render/model/Model.h"

#include <prev/render/buffer/BufferBuilder.h>

namespace prev_test::component::ray_casting {
namespace {
    class BoundingVolumeMesh : public prev_test::render::IMesh {
    public:
        BoundingVolumeMesh(const std::vector<glm::vec3>& vertices, const std::vector<uint32_t>& indices)
            : m_vertices{ vertices }
            , m_indices{ indices }
        {
            m_meshParts.push_back(prev_test::render::MeshPart(static_cast<uint32_t>(indices.size()), vertices));
        }

    public:
        const prev_test::render::VertexLayout& GetVertexLayout() const override
        {
            return vertexLayout;
        }

        const void* GetVertexData() const override
        {
            return m_vertices.data();
        }

        uint32_t GerVerticesCount() const override
        {
            return static_cast<uint32_t>(m_vertices.size());
        }

        const std::vector<uint32_t>& GetIndices() const override
        {
            return m_indices;
        }

        uint32_t GetIndicesCount() const override
        {
            return static_cast<uint32_t>(m_indices.size());
        }

        const std::vector<prev_test::render::MeshPart>& GetMeshParts() const override
        {
            return m_meshParts;
        }

        const prev_test::render::MeshNode& GetRootNode() const override
        {
            return meshRootNode;
        }

    private:
        std::vector<glm::vec3> m_vertices;

        std::vector<uint32_t> m_indices;

        std::vector<prev_test::render::MeshPart> m_meshParts;

        static const inline prev_test::render::VertexLayout vertexLayout{ { prev_test::render::VertexLayoutComponent::VEC3 } };

        static const inline prev_test::render::MeshNode meshRootNode{ { 0 }, glm::mat4(1.0f), {} };
    };

    std::shared_ptr<prev_test::render::IMesh> GenerateBoxMesh(const std::vector<glm::vec3>& boxPoints)
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

        return std::make_shared<BoundingVolumeMesh>(vertices, indices);
    }

    std::shared_ptr<prev_test::render::IMesh> GenerateSphereMesh(const prev_test::common::intersection::Sphere& sphere)
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
        return std::make_shared<BoundingVolumeMesh>(vertices, indices);
    }
} // namespace

BoundingVolumeModelFactory::BoundingVolumeModelFactory(prev::core::memory::Allocator& allocator)
    : m_allocator{ allocator }
{
}

std::unique_ptr<prev_test::render::IModel> BoundingVolumeModelFactory::CreateAABBModel(const prev_test::common::intersection::AABB& aabb) const
{
    const auto aabbPoints{ aabb.GetPoints() };
    auto boxMesh = GenerateBoxMesh(aabbPoints);

    const auto verticesDataSize{ boxMesh->GetVertexLayout().GetStride() * boxMesh->GerVerticesCount() };
    auto vertexBuffer = prev::render::buffer::BufferBuilder{ m_allocator }
                            .SetMemoryType(prev::core::memory::MemoryType::HOST_MAPPED)
                            .SetUsageFlags(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
                            .SetSize(verticesDataSize)
                            .SetData(boxMesh->GetVertexData(), verticesDataSize)
                            .Build();

    const auto indicesDataSize{ sizeof(uint32_t) * boxMesh->GetIndicesCount() };
    auto indexBuffer = prev::render::buffer::BufferBuilder{ m_allocator }
                           .SetMemoryType(prev::core::memory::MemoryType::HOST_MAPPED)
                           .SetUsageFlags(VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
                           .SetSize(indicesDataSize)
                           .SetData(boxMesh->GetIndices().data(), indicesDataSize)
                           .Build();

    return std::make_unique<prev_test::render::model::Model>(std::move(boxMesh), std::move(vertexBuffer), std::move(indexBuffer));
}

std::unique_ptr<prev_test::render::IModel> BoundingVolumeModelFactory::CreateAABBModel(const prev_test::common::intersection::AABB& aabb, const std::shared_ptr<prev_test::render::IModel>& model) const
{
    const auto aabbPoints{ aabb.GetPoints() };
    auto boxMesh = GenerateBoxMesh(aabbPoints);

    auto vertexBuffer{ model->GetVertexBuffer() };
    auto indexBuffer{ model->GetIndexBuffer() };
    vertexBuffer->Write(boxMesh->GetVertexData(), boxMesh->GetVertexLayout().GetStride() * boxMesh->GerVerticesCount());
    indexBuffer->Write(boxMesh->GetIndices().data(), sizeof(uint32_t) * boxMesh->GetIndicesCount());

    return std::make_unique<prev_test::render::model::Model>(std::move(boxMesh), std::move(vertexBuffer), std::move(indexBuffer));
}

std::unique_ptr<prev_test::render::IModel> BoundingVolumeModelFactory::CreateOBBModel(const prev_test::common::intersection::OBB& obb) const
{
    const auto obbPoints{ obb.GetPoints() };
    auto boxMesh = GenerateBoxMesh(obbPoints);

    const auto verticesDataSize{ boxMesh->GetVertexLayout().GetStride() * boxMesh->GerVerticesCount() };
    auto vertexBuffer = prev::render::buffer::BufferBuilder{ m_allocator }
                            .SetMemoryType(prev::core::memory::MemoryType::HOST_MAPPED)
                            .SetUsageFlags(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
                            .SetSize(verticesDataSize)
                            .SetData(boxMesh->GetVertexData(), verticesDataSize)
                            .Build();

    const auto indicesDataSize{ sizeof(uint32_t) * boxMesh->GetIndicesCount() };
    auto indexBuffer = prev::render::buffer::BufferBuilder{ m_allocator }
                           .SetMemoryType(prev::core::memory::MemoryType::HOST_MAPPED)
                           .SetUsageFlags(VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
                           .SetSize(indicesDataSize)
                           .SetData(boxMesh->GetIndices().data(), indicesDataSize)
                           .Build();

    return std::make_unique<prev_test::render::model::Model>(std::move(boxMesh), std::move(vertexBuffer), std::move(indexBuffer));
}

std::unique_ptr<prev_test::render::IModel> BoundingVolumeModelFactory::CreateOBBModel(const prev_test::common::intersection::OBB& obb, const std::shared_ptr<prev_test::render::IModel>& model) const
{
    const auto obbPoints{ obb.GetPoints() };
    auto boxMesh = GenerateBoxMesh(obbPoints);

    auto vertexBuffer{ model->GetVertexBuffer() };
    auto indexBuffer{ model->GetIndexBuffer() };
    vertexBuffer->Write(boxMesh->GetVertexData(), boxMesh->GetVertexLayout().GetStride() * boxMesh->GerVerticesCount());
    indexBuffer->Write(boxMesh->GetIndices().data(), sizeof(uint32_t) * boxMesh->GetIndicesCount());

    return std::make_unique<prev_test::render::model::Model>(std::move(boxMesh), std::move(vertexBuffer), std::move(indexBuffer));
}

std::unique_ptr<prev_test::render::IModel> BoundingVolumeModelFactory::CreateSphereModel(const prev_test::common::intersection::Sphere& sphere) const
{
    auto sphereMesh = GenerateSphereMesh(sphere);

    const auto verticesDataSize{ sphereMesh->GetVertexLayout().GetStride() * sphereMesh->GerVerticesCount() };
    auto vertexBuffer = prev::render::buffer::BufferBuilder{ m_allocator }
                            .SetMemoryType(prev::core::memory::MemoryType::HOST_MAPPED)
                            .SetUsageFlags(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
                            .SetSize(verticesDataSize)
                            .SetData(sphereMesh->GetVertexData(), verticesDataSize)
                            .Build();

    const auto indicesDataSize{ sizeof(uint32_t) * sphereMesh->GetIndices().size() };
    auto indexBuffer = prev::render::buffer::BufferBuilder{ m_allocator }
                           .SetMemoryType(prev::core::memory::MemoryType::HOST_MAPPED)
                           .SetUsageFlags(VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
                           .SetSize(indicesDataSize)
                           .SetData(sphereMesh->GetIndices().data(), indicesDataSize)
                           .Build();

    return std::make_unique<prev_test::render::model::Model>(std::move(sphereMesh), std::move(vertexBuffer), std::move(indexBuffer));
}

std::unique_ptr<prev_test::render::IModel> BoundingVolumeModelFactory::CreateSphereModel(const prev_test::common::intersection::Sphere& sphere, const std::shared_ptr<prev_test::render::IModel>& model) const
{
    auto sphereMesh = GenerateSphereMesh(sphere);

    auto vertexBuffer{ model->GetVertexBuffer() };
    auto indexBuffer{ model->GetIndexBuffer() };
    vertexBuffer->Write(sphereMesh->GetVertexData(), sphereMesh->GetVertexLayout().GetStride() * sphereMesh->GetIndicesCount());
    indexBuffer->Write(sphereMesh->GetIndices().data(), sizeof(uint32_t) * sphereMesh->GetIndicesCount());

    return std::make_unique<prev_test::render::model::Model>(std::move(sphereMesh), std::move(vertexBuffer), std::move(indexBuffer));
}
} // namespace prev_test::component::ray_casting