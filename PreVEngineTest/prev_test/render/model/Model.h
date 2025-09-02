#ifndef __MODEL_H__
#define __MODEL_H__

#include "../IModel.h"

#include <memory>

namespace prev_test::render::model {
class Model : public IModel {
public:
    Model(const std::shared_ptr<IMesh>& mesh, const std::shared_ptr<prev::render::buffer::Buffer>& vbo, const std::shared_ptr<prev::render::buffer::Buffer>& ibo);

    virtual ~Model() = default;

public:
    std::shared_ptr<IMesh> GetMesh() const override;

    std::shared_ptr<prev::render::buffer::Buffer> GetVertexBuffer() const override;

    std::shared_ptr<prev::render::buffer::Buffer> GetIndexBuffer() const override;

private:
    std::shared_ptr<IMesh> m_mesh;

    std::shared_ptr<prev::render::buffer::Buffer> m_vbo;

    std::shared_ptr<prev::render::buffer::Buffer> m_ibo;
};
} // namespace prev_test::render::model

#endif // !__MODEL_H__
