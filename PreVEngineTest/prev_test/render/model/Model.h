#ifndef __MODEL_H__
#define __MODEL_H__

#include "../IModel.h"

#include <memory>

namespace prev_test::render::model {
class Model : public IModel {
public:
    Model(const std::shared_ptr<IMesh>& mesh, const std::shared_ptr<prev::render::buffer::VertexBuffer>& vbo, const std::shared_ptr<prev::render::buffer::IndexBuffer>& ibo);

    virtual ~Model() = default;

public:
    std::shared_ptr<IMesh> GetMesh() const override;

    std::shared_ptr<prev::render::buffer::VertexBuffer> GetVertexBuffer() const override;

    std::shared_ptr<prev::render::buffer::IndexBuffer> GetIndexBuffer() const override;

private:
    std::shared_ptr<IMesh> m_mesh;

    std::shared_ptr<prev::render::buffer::VertexBuffer> m_vbo;

    std::shared_ptr<prev::render::buffer::IndexBuffer> m_ibo;
};
} // namespace prev_test::render::model

#endif // !__MODEL_H__
