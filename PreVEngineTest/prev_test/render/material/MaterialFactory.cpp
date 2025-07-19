#include "MaterialFactory.h"
#include "Material.h"

#include "../util/assimp/AssimpSceneLoader.h"

#include <prev/common/Cache.h>
#include <prev/render/buffer/ImageBufferBuilder.h>
#include <prev/render/image/ImageFactory.h>

#include <prev/util/VkUtils.h>

#include <stdexcept>

namespace prev_test::render::material {
namespace {
    using ImageCache = prev::common::Cache<std::string, std::shared_ptr<prev::render::image::IImage>>;

    static ImageCache s_imagesCache;

    std::shared_ptr<prev::render::image::IImage> CreateImage(const aiTexture& texture)
    {
        auto image = prev::render::image::ImageFactory{}.CreateImageFromMemory(reinterpret_cast<uint8_t*>(texture.pcData), texture.mWidth);
        return image;
    }

    std::shared_ptr<prev::render::image::IImage> CreateImage(const std::string& textureFilename)
    {
        std::shared_ptr<prev::render::image::IImage> image;
        if (auto optValue = s_imagesCache.Find(textureFilename)) {
            image = *optValue;
        } else {
            image = prev::render::image::ImageFactory{}.CreateImage(textureFilename);
            s_imagesCache.Add(textureFilename, image);
        }
        return image;
    }

    std::shared_ptr<prev::render::image::IImage> CreateModelImage(const aiScene& scene, const aiMaterial& material, const aiTextureType textureType)
    {
        aiString textureFilePath;
        if (material.Get(AI_MATKEY_TEXTURE(textureType, 0), textureFilePath) != aiReturn_SUCCESS) {
            return nullptr;
        }

        if (auto texture = scene.GetEmbeddedTexture(textureFilePath.C_Str())) {
            return CreateImage(*texture);
        } else {
            return CreateImage(textureFilePath.C_Str());
        }
    }

    enum class ImageBufferViewType {
        REGULAR = 0,
        CUBE_MAP = 1,
    };

    std::shared_ptr<prev::render::buffer::ImageBuffer> CreateImageBuffer(const std::vector<std::shared_ptr<prev::render::image::IImage>>& images, const ImageBufferViewType& imageViewType, const bool generateMipMaps, prev::core::memory::Allocator& allocator)
    {
        std::vector<const uint8_t*> layersData{};
        for (const auto& image : images) {
            layersData.emplace_back(image->GetRawDataPtr());
        }

        VkImageCreateFlags createFlags{};
        VkImageViewType viewType{ VK_IMAGE_VIEW_TYPE_2D };
        if (imageViewType == ImageBufferViewType::CUBE_MAP) {
            createFlags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
            viewType = VK_IMAGE_VIEW_TYPE_CUBE;
        }

        auto imageBuffer = prev::render::buffer::ImageBufferBuilder{ allocator }
                               .SetExtent({ images[0]->GetWidth(), images[0]->GetHeight(), 1 })
                               .SetFormat(prev::util::vk::ToImageFormat(images[0]->GetChannels(), images[0]->GetBitDepth(), images[0]->IsFloatingPoint()))
                               .SetType(VK_IMAGE_TYPE_2D)
                               .SetMipMapEnabled(generateMipMaps)
                               .SetUsageFlags(VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
                               .SetCreateFlags(createFlags)
                               .SetLayerData(layersData)
                               .SetViewType(viewType)
                               .SetLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
                               .Build();
        return imageBuffer;
    }

} // namespace

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
    auto imageBuffer{ CreateImageBuffer({ image }, ImageBufferViewType::REGULAR, true, m_allocator) };

    return std::make_unique<prev_test::render::material::Material>(materialProps, std::vector<std::shared_ptr<prev::render::buffer::ImageBuffer>>{ imageBuffer });
}

std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::Create(const MaterialProperties& materialProps, const std::string& colorImagePath, const std::string& normalMapPath) const
{
    auto image{ CreateImage(colorImagePath) };
    auto imageBuffer{ CreateImageBuffer({ image }, ImageBufferViewType::REGULAR, true, m_allocator) };

    auto normalImage{ CreateImage(normalMapPath) };
    auto normalImageBuffer{ CreateImageBuffer({ normalImage }, ImageBufferViewType::REGULAR, true, m_allocator) };

    return std::make_unique<prev_test::render::material::Material>(materialProps, std::vector<std::shared_ptr<prev::render::buffer::ImageBuffer>>{ imageBuffer, normalImageBuffer });
}

std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::Create(const MaterialProperties& materialProps, const std::string& colorImagePath, const std::string& normalMapPath, const std::string& heightMapPath) const
{
    auto image{ CreateImage(colorImagePath) };
    auto imageBuffer{ CreateImageBuffer({ image }, ImageBufferViewType::REGULAR, true, m_allocator) };

    auto normalImage{ CreateImage(normalMapPath) };
    auto normalImageBuffer{ CreateImageBuffer({ normalImage }, ImageBufferViewType::REGULAR, true, m_allocator) };

    auto heightImage{ CreateImage(heightMapPath) };
    auto heightImageBuffer{ CreateImageBuffer({ heightImage }, ImageBufferViewType::REGULAR, true, m_allocator) };

    return std::make_unique<prev_test::render::material::Material>(materialProps, std::vector<std::shared_ptr<prev::render::buffer::ImageBuffer>>{ imageBuffer, normalImageBuffer, heightImageBuffer });
}

std::unique_ptr<prev_test::render::IMaterial> MaterialFactory::CreateCubeMap(const MaterialProperties& materialProps, const std::vector<std::string>& sidePaths) const
{
    prev::render::image::ImageFactory imageFactory{};

    std::vector<std::shared_ptr<prev::render::image::IImage>> images{};
    for (const auto& faceFilePath : sidePaths) {
        images.emplace_back(imageFactory.CreateImage(faceFilePath));
    }

    auto cubeMapImageBuffer{ CreateImageBuffer(images, ImageBufferViewType::CUBE_MAP, true, m_allocator) };

    return std::make_unique<prev_test::render::material::Material>(materialProps, std::vector<std::shared_ptr<prev::render::buffer::ImageBuffer>>{ std::move(cubeMapImageBuffer) });
}

std::vector<std::shared_ptr<prev_test::render::IMaterial>> MaterialFactory::Create(const std::string& modelPath) const
{
    std::vector<std::shared_ptr<prev_test::render::IMaterial>> result;

    Assimp::Importer importer{};
    const aiScene* scene{};

    prev_test::render::util::assimp::AssimpSceneLoader assimpSceneLoader{};
    if (!assimpSceneLoader.LoadScene(modelPath, importer, &scene)) {
        throw std::runtime_error("Could not load model: " + modelPath);
    }

    for (uint32_t i = 0; i < scene->mNumMaterials; ++i) {
        const auto& material{ *scene->mMaterials[i] };

        std::vector<std::shared_ptr<prev::render::buffer::ImageBuffer>> imageBuffers;

        const aiTextureType textureTypes[]{
            aiTextureType_DIFFUSE,
            aiTextureType_NORMALS,
            aiTextureType_HEIGHT
        };

        for (const auto& textureType : textureTypes) {
            if (auto image = CreateModelImage(*scene, material, textureType)) {
                imageBuffers.emplace_back(CreateImageBuffer({ image }, ImageBufferViewType::REGULAR, true, m_allocator));
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

} // namespace prev_test::render::material
