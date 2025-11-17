#include "BoundingVolumeModelFactory.h"

#include "../../render/mesh/MeshFactory.h"
#include "../../render/model/ModelFactory.h"

#include <prev/render/buffer/BufferBuilder.h>

namespace prev_test::component::ray_casting {
namespace {
    std::unique_ptr<prev_test::render::IMesh> GenerateBoxMesh(const std::vector<glm::vec3>& boxPoints)
    {
        const std::vector<uint32_t> indices = {
            3, 2, 6, 6, 7, 3,
            1, 5, 4, 4, 0, 1,
            3, 7, 5, 5, 1, 3,
            2, 0, 4, 4, 6, 2,
            2, 3, 1, 1, 0, 2,
            6, 4, 5, 5, 7, 0
        };

        const std::vector<glm::vec2> textureCoords(boxPoints.size(), glm::vec2(0.0f, 0.0f));
        const std::vector<glm::vec3> normals(boxPoints.size(), glm::vec3(0.0f, 0.0f, 0.0f));

        return prev_test::render::mesh::MeshFactory{}.CreateFromData(boxPoints, textureCoords, normals, indices);
    }

    std::unique_ptr<prev_test::render::IModel> CreateModelFromMesh(std::unique_ptr<prev_test::render::IMesh> mesh, const std::shared_ptr<prev_test::render::IModel>& model, prev::core::memory::Allocator& allocator)
    {
        auto vertexBuffer{ model->GetVertexBuffer() };
        auto indexBuffer{ model->GetIndexBuffer() };
        vertexBuffer->Write(mesh->GetVertexData(), mesh->GetVertexLayout().GetStride() * mesh->GerVerticesCount());
        indexBuffer->Write(mesh->GetIndices().data(), sizeof(uint32_t) * mesh->GetIndicesCount());

        return prev_test::render::model::ModelFactory{ allocator }.Create(std::move(mesh), std::move(vertexBuffer), std::move(indexBuffer));
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
    return prev_test::render::model::ModelFactory{ m_allocator }.CreateHostVisible(std::move(boxMesh));
}

std::unique_ptr<prev_test::render::IModel> BoundingVolumeModelFactory::CreateAABBModel(const prev_test::common::intersection::AABB& aabb, const std::shared_ptr<prev_test::render::IModel>& model) const
{
    const auto aabbPoints{ aabb.GetPoints() };
    auto boxMesh = GenerateBoxMesh(aabbPoints);
    return CreateModelFromMesh(std::move(boxMesh), model, m_allocator);
}

std::unique_ptr<prev_test::render::IModel> BoundingVolumeModelFactory::CreateOBBModel(const prev_test::common::intersection::OBB& obb) const
{
    const auto obbPoints{ obb.GetPoints() };
    auto boxMesh = GenerateBoxMesh(obbPoints);
    return prev_test::render::model::ModelFactory{ m_allocator }.CreateHostVisible(std::move(boxMesh));
}

std::unique_ptr<prev_test::render::IModel> BoundingVolumeModelFactory::CreateOBBModel(const prev_test::common::intersection::OBB& obb, const std::shared_ptr<prev_test::render::IModel>& model) const
{
    const auto obbPoints{ obb.GetPoints() };
    auto boxMesh = GenerateBoxMesh(obbPoints);
    return CreateModelFromMesh(std::move(boxMesh), model, m_allocator);
}

std::unique_ptr<prev_test::render::IModel> BoundingVolumeModelFactory::CreateSphereModel(const prev_test::common::intersection::Sphere& sphere) const
{
    auto sphereMesh = prev_test::render::mesh::MeshFactory{}.CreateSphere(sphere.radius, 16, 16, 360.0f, 180.0f, sphere.position, false);
    return prev_test::render::model::ModelFactory{ m_allocator }.CreateHostVisible(std::move(sphereMesh));
}

std::unique_ptr<prev_test::render::IModel> BoundingVolumeModelFactory::CreateSphereModel(const prev_test::common::intersection::Sphere& sphere, const std::shared_ptr<prev_test::render::IModel>& model) const
{
    auto sphereMesh = prev_test::render::mesh::MeshFactory{}.CreateSphere(sphere.radius, 16, 16, 360.0f, 180.0f, sphere.position, false);
    return CreateModelFromMesh(std::move(sphereMesh), model, m_allocator);
}
} // namespace prev_test::component::ray_casting