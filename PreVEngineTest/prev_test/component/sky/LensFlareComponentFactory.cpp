#include "LensFlareComponentFactory.h"
#include "LensFlareComponent.h"

#include "../../common/AssetManager.h"
#include "../../render/mesh/MeshFactory.h"
#include "../../render/model/ModelFactory.h"

#include <prev/render/buffer/ImageBufferBuilder.h>
#include <prev/render/sampler/Sampler.h>
#include <prev/util/VkUtils.h>

namespace prev_test::component::sky {
LensFlareComponentFactory::LensFlareComponentFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator)
    : m_device{ device }
    , m_allocator{ allocator }
{
}

std::unique_ptr<ILensFlareComponent> LensFlareComponentFactory::Create() const
{
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

    std::vector<std::shared_ptr<Flare>> flares{};
    for (const auto& flareCreateInfo : flareCreateInfos) {
        auto flare = CreateFlare(flareCreateInfo.path, flareCreateInfo.scale);
        flares.emplace_back(std::move(flare));
    }

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh{ meshFactory.CreateQuad() };

    prev_test::render::model::ModelFactory modelFactory{ m_allocator };
    auto model{ modelFactory.Create(std::move(mesh)) };

    return std::make_unique<LensFlareComponent>(flares, spacing, std::move(model));
}

std::unique_ptr<Flare> LensFlareComponentFactory::CreateFlare(const std::string& filePath, const float scale) const
{
    auto image{ prev::render::image::ImageFactory{}.CreateImage(filePath) };
    auto imageBuffer = prev::render::buffer::ImageBufferBuilder{ m_allocator }
                           .SetExtent({ image->GetWidth(), image->GetHeight(), 1 })
                           .SetFormat(VK_FORMAT_R8G8B8A8_UNORM)
                           .SetType(VK_IMAGE_TYPE_2D)
                           .SetMipMapEnabled(true)
                           .SetUsageFlags(VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
                           .SetLayerData({ reinterpret_cast<uint8_t*>(image->GetBuffer()) })
                           .SetLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
                           .Build();
    auto sampler{ std::make_shared<prev::render::sampler::Sampler>(m_device, static_cast<float>(imageBuffer->GetMipLevels()), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, true, 16.0f) };
    return std::make_unique<Flare>(m_device, std::move(imageBuffer), sampler, scale);
}
} // namespace prev_test::component::sky