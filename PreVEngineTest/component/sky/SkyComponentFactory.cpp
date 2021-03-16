#include "SkyComponentFactory.h"
#include "SkyCommon.h"
#include "SkyComponent.h"

#include "../../render/mesh/MeshFactory.h"
#include "../../render/model/ModelFactory.h"

#include <prev/core/AllocatorProvider.h>

namespace prev_test::component::sky {
std::unique_ptr<ISkyComponent> SkyComponentFactory::Create() const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh{ meshFactory.CreateFullScreenQuad() };

    prev_test::render::model::ModelFactory modelFactory{};
    auto model{ modelFactory.Create(std::move(mesh), *allocator) };

    auto sky{ std::make_unique<SkyComponent>() };
    sky->m_model = std::move(model);
    sky->m_bottomColor = glm::vec3(FOG_COLOR.x, FOG_COLOR.y, FOG_COLOR.z);
    sky->m_topColor = glm::vec3(0.521f, 0.619f, 0.839);
    return sky;
}
} // namespace prev_test::component::sky