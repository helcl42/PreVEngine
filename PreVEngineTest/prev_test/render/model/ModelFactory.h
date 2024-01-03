#ifndef __MODEL_FACTORY_H__
#define __MODEL_FACTORY_H__

#include "../IModel.h"

#include <memory>

namespace prev_test::render::model {
class ModelFactory final {
public:
    std::unique_ptr<prev_test::render::IModel> Create(const std::shared_ptr<IMesh>& mesh, prev::core::memory::Allocator& allocator) const;

    std::unique_ptr<prev_test::render::IModel> Create(const std::shared_ptr<IMesh>& mesh, const std::shared_ptr<prev::render::buffer::VertexBuffer>& vertexBuffer, const std::shared_ptr<prev::render::buffer::IndexBuffer>& indexBuffer) const;

    std::unique_ptr<prev_test::render::IModel> CreateHostVisible(const std::shared_ptr<IMesh>& mesh, const uint32_t maxVertexCount, const uint32_t maxIndexCount, prev::core::memory::Allocator& allocator) const;
};
} // namespace prev_test::render::model

#endif // !__MODEL_FACTORY_H__
