#include "SkyComponentFactory.h"
#include "../../render/mesh/FullScreenQuadMesh.h"
#include "../../render/model/ModelFactory.h"
#include "SkyCommon.h"
#include "SkyComponent.h"

#include <prev/scene/AllocatorProvider.h>

namespace prev_test::component::sky {
std::unique_ptr<ISkyComponent> SkyComponentFactory::Create() const
{
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    auto sky = std::make_unique<SkyComponent>();
    sky->m_model = CreateModel(*allocator);
    sky->m_bottomColor = glm::vec3(FOG_COLOR.x, FOG_COLOR.y, FOG_COLOR.z);
    sky->m_topColor = glm::vec3(0.521f, 0.619f, 0.839);
    return sky;
}

std::unique_ptr<prev_test::render::IModel> SkyComponentFactory::CreateModel(prev::core::memory::Allocator& allocator) const
{
    auto mesh = std::make_unique<prev_test::render::mesh::FullScreenQuadMesh>();
    auto vertexBuffer = std::make_unique<prev::core::memory::buffer::VertexBuffer>(allocator);
    vertexBuffer->Data(mesh->GetVertexData(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());
    auto indexBuffer = std::make_unique<prev::core::memory::buffer::IndexBuffer>(allocator);
    indexBuffer->Data(mesh->GetIndices().data(), static_cast<uint32_t>(mesh->GetIndices().size()));

    prev_test::render::model::ModelFactory modelFactory{};
    return modelFactory.Create(std::move(mesh), std::move(vertexBuffer), std::move(indexBuffer));
}
} // namespace prev_test::component::sky