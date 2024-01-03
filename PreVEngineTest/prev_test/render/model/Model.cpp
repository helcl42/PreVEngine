#include "Model.h"

namespace prev_test::render::model {
Model::Model(const std::shared_ptr<IMesh>& mesh, const std::shared_ptr<prev::render::buffer::VertexBuffer>& vbo, const std::shared_ptr<prev::render::buffer::IndexBuffer>& ibo)
    : m_mesh(mesh)
    , m_vbo(vbo)
    , m_ibo(ibo)
{
}

std::shared_ptr<IMesh> Model::GetMesh() const
{
    return m_mesh;
}

std::shared_ptr<prev::render::buffer::VertexBuffer> Model::GetVertexBuffer() const
{
    return m_vbo;
}

std::shared_ptr<prev::render::buffer::IndexBuffer> Model::GetIndexBuffer() const
{
    return m_ibo;
}
} // namespace prev_test::render::model
