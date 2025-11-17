#include "LensFlareComponentFactory.h"
#include "LensFlareComponent.h"

#include "../../common/AssetManager.h"
#include "../../render/material/MaterialFactory.h"
#include "../../render/mesh/MeshFactory.h"
#include "../../render/model/ModelFactory.h"

namespace prev_test::component::sky {
LensFlareComponentFactory::LensFlareComponentFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator)
    : m_device{ device }
    , m_allocator{ allocator }
{
}

std::unique_ptr<ILensFlareComponent> LensFlareComponentFactory::Create() const
{
    struct FlareCreateInfo {
        std::string path{};
        float scale{};
    };

    const float spacing{ 0.16f };
    const std::vector<FlareCreateInfo> flareCreateInfos = {
        { prev_test::common::AssetManager::Instance().GetAssetPath("LensFlares/tex2.png"), 0.0f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("LensFlares/tex3.png"), 0.12f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("LensFlares/tex4.png"), 0.46f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("LensFlares/tex5.png"), 0.12f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("LensFlares/tex6.png"), 0.0f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("LensFlares/tex7.png"), 0.1f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("LensFlares/tex8.png"), 1.2f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("LensFlares/tex9.png"), 0.24f },
    };

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh{ meshFactory.CreateQuad() };

    prev_test::render::model::ModelFactory modelFactory{ m_allocator };
    auto model{ modelFactory.Create(std::move(mesh)) };

    prev_test::render::material::MaterialFactory materialFactory{ m_device, m_allocator };

    std::vector<std::shared_ptr<prev_test::render::IMaterial>> materials{};
    std::vector<Flare> flares{};
    for (const auto& flareCreateInfo : flareCreateInfos) {
        const prev_test::render::MaterialProperties flareMaterialProperties{ { 1.0f, 1.0f, 1.0f, 1.0f }, 1.0f, 1.0f };
        materials.emplace_back(materialFactory.Create(flareMaterialProperties, flareCreateInfo.path));
        flares.emplace_back(Flare{ flareCreateInfo.scale });
    }

    return std::make_unique<LensFlareComponent>(flares, spacing, materials, std::move(model));
}
} // namespace prev_test::component::sky