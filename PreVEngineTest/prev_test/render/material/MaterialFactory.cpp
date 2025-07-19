#include "MaterialFactory.h"
#include "Material.h"

#include "../util/assimp/AssimpMaterialFactory.h"
#include "../util/assimp/AssimpSceneLoader.h"

#include <prev/render/buffer/ImageBufferBuilder.h>
#include <prev/render/image/ImageFactory.h>

#include <prev/util/VkUtils.h>

#include <stdexcept>

namespace prev_test::render::material {
MaterialFactory::MaterialFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator)
    : m_device{ device }
    , m_allocator{ allocator }
{
}

std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::Create(const MaterialProperties& materialProps) const
{
    return std::make_unique<prev_test::render::material::Material>(materialProps);
}

std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::Create(const MaterialProperties& materialProps, const std::string& colorImagePath) const
{
    auto image{ CreateImage(colorImagePath) };
    auto imageBuffer{ CreateImageBuffer(*image, true) };

    return std::make_unique<prev_test::render::material::Material>(materialProps, std::vector<std::shared_ptr<prev::render::buffer::ImageBuffer>>{ imageBuffer });
}

std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::Create(const MaterialProperties& materialProps, const std::string& colorImagePath, const std::string& normalMapPath) const
{
    auto image{ CreateImage(colorImagePath) };
    auto imageBuffer{ CreateImageBuffer(*image, true) };

    auto normalImage{ CreateImage(normalMapPath) };
    auto normalImageBuffer{ CreateImageBuffer(*normalImage, true) };

    return std::make_unique<prev_test::render::material::Material>(materialProps, std::vector<std::shared_ptr<prev::render::buffer::ImageBuffer>>{ imageBuffer, normalImageBuffer });
}

std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::Create(const MaterialProperties& materialProps, const std::string& colorImagePath, const std::string& normalMapPath, const std::string& heightMapPath) const
{
    auto image{ CreateImage(colorImagePath) };
    auto imageBuffer{ CreateImageBuffer(*image, true) };

    auto normalImage{ CreateImage(normalMapPath) };
    auto normalImageBuffer{ CreateImageBuffer(*normalImage, true) };

    auto heightImage{ CreateImage(heightMapPath) };
    auto heightImageBuffer{ CreateImageBuffer(*heightImage, true) };

    return std::make_unique<prev_test::render::material::Material>(materialProps, std::vector<std::shared_ptr<prev::render::buffer::ImageBuffer>>{ imageBuffer, normalImageBuffer, heightImageBuffer });
}

std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::CreateCubeMap(const MaterialProperties& materialProps, const std::vector<std::string>& sidePaths) const
{
    prev::render::image::ImageFactory imageFactory{};

    std::vector<std::shared_ptr<prev::render::image::IImage>> images{};
    for (const auto& faceFilePath : sidePaths) {
        images.emplace_back(imageFactory.CreateImage(faceFilePath));
    }

    std::vector<const uint8_t*> layersData{};
    for (const auto& image : images) {
        layersData.emplace_back(image->GetRawDataPtr());
    }

    auto cubeMapImageBuffer = prev::render::buffer::ImageBufferBuilder{ m_allocator }
                                  .SetExtent({ images[0]->GetWidth(), images[0]->GetHeight(), 1 })
                                  .SetFormat(prev::util::vk::ToImageFormat(images[0]->GetChannels(), images[0]->GetBitDepth(), images[0]->IsFloatingPoint()))
                                  .SetType(VK_IMAGE_TYPE_2D)
                                  .SetMipMapEnabled(true)
                                  .SetUsageFlags(VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
                                  .SetCreateFlags(VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT)
                                  .SetLayerData(layersData)
                                  .SetLayerCount(static_cast<uint32_t>(images.size()))
                                  .SetViewType(VK_IMAGE_VIEW_TYPE_CUBE)
                                  .SetLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
                                  .Build();

    return std::make_unique<prev_test::render::material::Material>(materialProps, std::vector<std::shared_ptr<prev::render::buffer::ImageBuffer>>{ std::move(cubeMapImageBuffer) });
}

std::vector<std::shared_ptr<prev_test::render::IMaterial>> MaterialFactory::Create(const std::string& modelPath) const
{
    std::vector<std::shared_ptr<prev_test::render::IMaterial>> result;

    Assimp::Importer importer{};
    const aiScene* scene{};

    prev_test::render::util::assimp::AssimpSceneLoader assimpSceneLoader{};
    if (!assimpSceneLoader.LoadScene(modelPath, &importer, &scene)) {
        throw std::runtime_error("Could not load model: " + modelPath);
    }

    prev_test::render::util::assimp::AssimpMaterialFactory assimpMaterialFactory{ s_imagesCache };
    for (uint32_t i = 0; i < scene->mNumMaterials; ++i) {
        const auto& material{ *scene->mMaterials[i] };

        std::vector<std::shared_ptr<prev::render::buffer::ImageBuffer>> imageBuffers;

        const aiTextureType textureTypes[]{
            aiTextureType_DIFFUSE,
            aiTextureType_NORMALS,
            aiTextureType_HEIGHT
        };

        for (const auto& textureType : textureTypes) {
            if (auto image = assimpMaterialFactory.CreateModelImage(*scene, material, textureType)) {
                imageBuffers.emplace_back(CreateImageBuffer(*image, true));
            }
        }

        aiColor3D color(1.0f, 1.0f, 1.0f);
        material.Get(AI_MATKEY_COLOR_DIFFUSE, color);

        float shineness{ 1.0f };
        material.Get(AI_MATKEY_SHININESS, shineness);

        float reflectivity{ 1.0f };
        material.Get(AI_MATKEY_REFLECTIVITY, reflectivity);

        const MaterialProperties materialProperties{ { color.r, color.g, color.b, 1.0f }, shineness, std::max(reflectivity, 1.0f) };

        result.emplace_back(std::make_unique<prev_test::render::material::Material>(materialProperties, imageBuffers));
    }

    return result;
}

std::shared_ptr<prev::render::image::IImage> MaterialFactory::CreateImage(const std::string& textureFilename) const
{
    using ImageCache = prev::common::Cache<std::string, std::shared_ptr<prev::render::image::IImage>>;

    std::shared_ptr<prev::render::image::IImage> image;
    if(auto optValue = s_imagesCache.Find(textureFilename)) {
        image = *optValue;
    } else {
        image = prev::render::image::ImageFactory{}.CreateImage(textureFilename);
        s_imagesCache.Add(textureFilename, image);
    }
    return image;
}

std::shared_ptr<prev::render::buffer::ImageBuffer> MaterialFactory::CreateImageBuffer(const prev::render::image::IImage& image, const bool generateMipMaps) const
{
    auto imageBuffer = prev::render::buffer::ImageBufferBuilder{ m_allocator }
                           .SetExtent({ image.GetWidth(), image.GetHeight(), 1 })
                           .SetFormat(prev::util::vk::ToImageFormat(image.GetChannels(), image.GetBitDepth(), image.IsFloatingPoint()))
                           .SetType(VK_IMAGE_TYPE_2D)
                           .SetMipMapEnabled(generateMipMaps)
                           .SetUsageFlags(VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
                           .SetLayerData({ image.GetRawDataPtr() })
                           .SetLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
                           .Build();
    return imageBuffer;
}

} // namespace prev_test::render::material
