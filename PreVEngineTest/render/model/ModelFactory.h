#ifndef __MODEL_FACTORY_H__
#define __MODEL_FACTORY_H__

#include "../IModel.h"

#include <memory>

namespace prev_test::render::model {
class ModelFactory final {
public:
    std::unique_ptr<prev_test::render::IModel> Create(const std::shared_ptr<IMesh>& mesh, prev::core::memory::Allocator& allocator) const;
};
} // namespace prev_test::render::model

#endif // !__MODEL_FACTORY_H__
