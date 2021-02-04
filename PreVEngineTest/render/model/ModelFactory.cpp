#include "ModelFactory.h"

#include "Model.h"

namespace prev_test::render::model {
std::unique_ptr<prev_test::render::IModel> ModelFactory::Create(const std::shared_ptr<IMesh>& mesh, prev::core::memory::Allocator& allocator) const
{
    auto vertexBuffer{ std::make_unique<prev::core::memory::buffer::VertexBuffer>(allocator) };
    vertexBuffer->Data(mesh->GetVertexData(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());

    auto indexBuffer{ std::make_unique<prev::core::memory::buffer::IndexBuffer>(allocator) };
    indexBuffer->Data(mesh->GetIndices().data(), (uint32_t)mesh->GetIndices().size());

    return std::make_unique<prev_test::render::model::Model>(mesh, std::move(vertexBuffer), std::move(indexBuffer));
}
} // namespace prev_test::render::model