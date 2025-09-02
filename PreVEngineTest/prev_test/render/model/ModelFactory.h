#ifndef __MODEL_FACTORY_H__
#define __MODEL_FACTORY_H__

#include "../IModel.h"

#include <prev/core/memory/Allocator.h>

#include <memory>

namespace prev_test::render::model {
class ModelFactory final {
public:
    ModelFactory(prev::core::memory::Allocator& allocator);

    ~ModelFactory() = default;

public:
    std::unique_ptr<prev_test::render::IModel> Create(const std::shared_ptr<IMesh>& mesh) const;

    std::unique_ptr<prev_test::render::IModel> Create(const std::shared_ptr<IMesh>& mesh, const std::shared_ptr<prev::render::buffer::Buffer>& vertexBuffer, const std::shared_ptr<prev::render::buffer::Buffer>& indexBuffer) const;

    std::unique_ptr<prev_test::render::IModel> CreateHostVisible(const std::shared_ptr<IMesh>& mesh, const uint32_t maxVertexCount, const uint32_t maxIndexCount) const;

private:
    prev::core::memory::Allocator& m_allocator;
};
} // namespace prev_test::render::model

#endif // !__MODEL_FACTORY_H__
