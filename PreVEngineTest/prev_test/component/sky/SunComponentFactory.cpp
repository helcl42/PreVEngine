#include "SunComponentFactory.h"
#include "SunComponent.h"

#include "../../common/AssetManager.h"
#include "../../render/mesh/MeshFactory.h"
#include "../../render/model/ModelFactory.h"

#include <prev/render/buffer/ImageBufferBuilder.h>
#include <prev/render/sampler/Sampler.h>
#include <prev/util/VkUtils.h>

namespace prev_test::component::sky {
SunComponentFactory::SunComponentFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator)
    : m_device{ device }
    , m_allocator{ allocator }
{
}

std::unique_ptr<ISunComponent> SunComponentFactory::Create() const
{
    auto flare{ CreateFlare(prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sun.png"), 0.2f) };

    auto mesh{ prev_test::render::mesh::MeshFactory{}.CreateQuad() };
    auto model{ prev_test::render::model::ModelFactory{ m_allocator }.Create(std::move(mesh)) };

    return std::make_unique<SunComponent>(std::move(flare), std::move(model));
}

std::unique_ptr<Flare> SunComponentFactory::CreateFlare(const std::string& filePath, const float scale) const
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

    return std::make_unique<Flare>(m_device, std::move(imageBuffer), scale);
}
} // namespace prev_test::component::sky