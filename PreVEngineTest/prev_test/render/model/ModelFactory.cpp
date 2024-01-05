#include "ModelFactory.h"

#include "Model.h"

namespace prev_test::render::model {
std::unique_ptr<prev_test::render::IModel> ModelFactory::Create(const std::shared_ptr<IMesh>& mesh, prev::core::memory::Allocator& allocator) const
{
    auto vertexBuffer{ std::make_unique<prev::render::buffer::VertexBuffer>(allocator) };
    vertexBuffer->Data(mesh->GetVertexData(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());

    auto indexBuffer{ std::make_unique<prev::render::buffer::IndexBuffer>(allocator) };
    indexBuffer->Data(mesh->GetIndices().data(), static_cast<uint32_t>(mesh->GetIndices().size()));

    return std::make_unique<prev_test::render::model::Model>(mesh, std::move(vertexBuffer), std::move(indexBuffer));
}

std::unique_ptr<prev_test::render::IModel> ModelFactory::Create(const std::shared_ptr<IMesh>& mesh, const std::shared_ptr<prev::render::buffer::VertexBuffer>& vertexBuffer, const std::shared_ptr<prev::render::buffer::IndexBuffer>& indexBuffer) const
{
    return std::make_unique<prev_test::render::model::Model>(mesh, vertexBuffer, indexBuffer);
}

std::unique_ptr<prev_test::render::IModel> ModelFactory::CreateHostVisible(const std::shared_ptr<IMesh>& mesh, const uint32_t maxVertexCount, const uint32_t maxIndexCount, prev::core::memory::Allocator& allocator) const
{
    const uint32_t finalVertexCount{ std::max(mesh->GerVerticesCount(), maxVertexCount) };
    const uint32_t finalIndexCount{ std::max(static_cast<uint32_t>(mesh->GetIndices().size()), maxIndexCount) };

    auto vertexBuffer{ std::make_unique<prev::render::buffer::HostMappedVertexBuffer>(allocator, finalVertexCount) };
    vertexBuffer->Data(mesh->GetVertexData(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());

    auto indexBuffer{ std::make_unique<prev::render::buffer::HostMappedIndexBuffer>(allocator, finalIndexCount) };
    indexBuffer->Data(mesh->GetIndices().data(), static_cast<uint32_t>(mesh->GetIndices().size()));

    return std::make_unique<prev_test::render::model::Model>(mesh, std::move(vertexBuffer), std::move(indexBuffer));
}
} // namespace prev_test::render::model