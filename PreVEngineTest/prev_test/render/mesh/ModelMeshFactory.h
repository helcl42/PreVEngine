#ifndef __MODEL_MESH_FACTORY_H__
#define __MODEL_MESH_FACTORY_H__

#include "../IMesh.h"

#include <prev/common/FlagSet.h>

#include <memory>

namespace prev_test::render::mesh {
class ModelMeshFactory final {
public:
    enum class CreateFlags {
        ANIMATION,
        TANGENT_BITANGENT,
        _
    };

public:
    std::unique_ptr<prev_test::render::IMesh> Create(const std::string& modelPath, const prev::common::FlagSet<CreateFlags>& flags = prev::common::FlagSet<ModelMeshFactory::CreateFlags>{}) const;
};

} // namespace prev_test::render::mesh

#endif // !__MODEL_FACTORY_H__
