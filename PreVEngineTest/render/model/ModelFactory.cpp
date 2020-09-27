#include "ModelFactory.h"

#include "Model.h"

namespace prev_test::render::model {
std::unique_ptr<prev_test::render::IModel> ModelFactory::Create(const std::shared_ptr<IMesh>& mesh, const std::shared_ptr<prev::core::memory::buffer::VertexBuffer>& vbo, const std::shared_ptr<prev::core::memory::buffer::IndexBuffer>& ibo) const
{
    return std::make_unique<prev_test::render::model::Model>(mesh, vbo, ibo);
}
} // namespace prev_test::render::model