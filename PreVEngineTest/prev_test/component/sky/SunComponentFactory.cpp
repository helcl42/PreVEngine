#include "SunComponentFactory.h"
#include "SunComponent.h"

#include "../../common/AssetManager.h"
#include "../../render/material/MaterialFactory.h"
#include "../../render/mesh/MeshFactory.h"
#include "../../render/model/ModelFactory.h"

namespace prev_test::component::sky {
SunComponentFactory::SunComponentFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator)
    : m_device{ device }
    , m_allocator{ allocator }
{
}

std::unique_ptr<ISunComponent> SunComponentFactory::Create() const
{
    struct FlareCreateInfo {
        std::string path{};
        float scale{};
    };

    const FlareCreateInfo sunFlareCreateInfo{ prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sun.png"), 0.2f };

    auto mesh{ prev_test::render::mesh::MeshFactory{}.CreateQuad() };
    auto model{ prev_test::render::model::ModelFactory{ m_allocator }.Create(std::move(mesh)) };
    auto material{ prev_test::render::material::MaterialFactory{ m_device, m_allocator }.Create({ { 1.0f, 1.0f, 1.0f, 1.0f }, 1.0f, 1.0f }, sunFlareCreateInfo.path) };
    auto flare{ Flare{ sunFlareCreateInfo.scale } };

    return std::make_unique<SunComponent>(flare, std::move(material), std::move(model));
}
} // namespace prev_test::component::sky