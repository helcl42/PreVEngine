#include "TerrainComponentFactory.h"
#include "HeightGenerator.h"
#include "TerrainComponent.h"

#include "../../common/AssetManager.h"
#include "../../render/material/MaterialFactory.h"
#include "../../render/mesh/MeshFactory.h"
#include "../../render/mesh/MeshUtil.h"
#include "../../render/model/ModelFactory.h"

#include <prev/render/buffer/ImageBufferBuilder.h>
#include <prev/render/image/ImageFactory.h>
#include <prev/util/GfxUtils.h>

namespace prev_test::component::terrain {
TerrainComponentFactory::TerrainComponentFactory(prev::core::device::Device& device, const unsigned int seed, const unsigned int vertexCount)
    : m_device{ device }
    , m_seed{ seed }
    , m_vertexCount{ vertexCount }
{
}

std::unique_ptr<ITerrainComponent> TerrainComponentFactory::CreateRandomTerrain(const int x, const int z, const float size) const
{
    HeightGenerator heightGenerator(x, z, m_vertexCount, m_seed);
    const std::shared_ptr<HeightMapInfo> heightMap{ CreateHeightMap(heightGenerator) };
    const std::shared_ptr<VertexData> vertexData{ GenerateVertexData(heightMap, size) };

    prev_test::render::material::MaterialFactory materialFactory{ m_device };

    const float layerTransitionWidth{ 0.1f };
    const TerrainLayerCreateInfo terrainLayers[] = {
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fungus.png"), 20.0f, 0.05f, 0.2f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_grass.png"), 20.0f, 0.05f, 0.42f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/rock.png"), 20.0f, 0.05f, 0.62f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand.png"), 20.0f, 0.05f, 0.9f }
    };

    auto result{ std::make_unique<TerrainComponent>(x, z) };
    result->m_model = CreateModel(vertexData, false);
    result->m_heightsInfo = heightMap;

    std::vector<std::string> colorPaths;
    for (const auto& layer : terrainLayers) {
        result->m_materials.emplace_back(materialFactory.Create({ glm::vec4(1.0f), layer.shineDamper, layer.reflectivity }, layer.materialPath));
        result->m_heightSteps.emplace_back(layer.heightStep);
        colorPaths.push_back(layer.materialPath);
    }
    result->m_textureArrays[0] = CreateTextureArray(colorPaths);
    result->m_transitionRange = layerTransitionWidth;
    return result;
}

std::unique_ptr<ITerrainComponent> TerrainComponentFactory::CreateRandomTerrainNormalMapped(const int x, const int z, const float size) const
{
    HeightGenerator heightGenerator(x, z, m_vertexCount, m_seed);
    const std::shared_ptr<HeightMapInfo> heightMap{ CreateHeightMap(heightGenerator) };
    const std::shared_ptr<VertexData> vertexData{ GenerateVertexData(heightMap, size) };

    prev_test::render::material::MaterialFactory materialFactory{ m_device };

    const float layerTransitionWidth{ 0.1f };
    const TerrainLayerCreateInfo terrainLayers[] = {
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fungus.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fungus_normal.png"), 20.0f, 0.05f, 0.2f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_grass.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_grass_normal.png"), 20.0f, 0.05f, 0.42f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/rock.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/rock_normal.png"), 20.0f, 0.05f, 0.62f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_normal.png"), 20.0f, 0.05f, 0.9f }
    };

    auto result{ std::make_unique<TerrainComponent>(x, z) };
    result->m_model = CreateModel(vertexData, true);
    result->m_heightsInfo = heightMap;

    std::vector<std::string> colorPaths;
    std::vector<std::string> normalPaths;
    for (const auto& layer : terrainLayers) {
        result->m_materials.emplace_back(materialFactory.Create({ glm::vec4(1.0f), layer.shineDamper, layer.reflectivity }, layer.materialPath, layer.materialNormalPath));
        result->m_heightSteps.emplace_back(layer.heightStep);
        colorPaths.push_back(layer.materialPath);
        normalPaths.push_back(layer.materialNormalPath);
    }
    result->m_textureArrays[0] = CreateTextureArray(colorPaths);
    result->m_textureArrays[1] = CreateTextureArray(normalPaths);
    result->m_transitionRange = layerTransitionWidth;
    return result;
}

std::unique_ptr<ITerrainComponent> TerrainComponentFactory::CreateRandomTerrainConeStepMapped(const int x, const int z, const float size) const
{
    HeightGenerator heightGenerator(x, z, m_vertexCount, m_seed);
    const std::shared_ptr<HeightMapInfo> heightMap{ CreateHeightMap(heightGenerator) };
    const std::shared_ptr<VertexData> vertexData{ GenerateVertexData(heightMap, size) };

    prev_test::render::material::MaterialFactory materialFactory{ m_device };

    const float layerTransitionWidth{ 0.1f };
    const TerrainLayerCreateInfo terrainLayers[] = {
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fungus.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fungus_normal.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/fungus_cone.png"), 20.0f, 0.05f, 0.00002f, 0.2f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_grass.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_grass_normal.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_grass_cone.png"), 20.0f, 0.05f, 0.01f, 0.42f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/rock.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/rock_normal.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/rock_cone.png"), 20.0f, 0.05f, 0.03f, 0.62f },
        { prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_normal.png"), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/sand_cone.png"), 20.0f, 0.05f, 0.05f, 0.9f }
    };

    auto result{ std::make_unique<TerrainComponent>(x, z) };
    result->m_model = CreateModel(vertexData, true);
    result->m_heightsInfo = heightMap;

    std::vector<std::string> colorPaths;
    std::vector<std::string> normalPaths;
    std::vector<std::string> heightPaths;
    for (const auto& layer : terrainLayers) {
        auto material{ materialFactory.Create({ glm::vec4(1.0f), layer.shineDamper, layer.reflectivity }, layer.materialPath, layer.materialNormalPath, layer.materialHeightPath) };
        material->SetHeightScale(layer.heightScale);
        result->m_materials.emplace_back(std::move(material));
        result->m_heightSteps.emplace_back(layer.heightStep);
        colorPaths.push_back(layer.materialPath);
        normalPaths.push_back(layer.materialNormalPath);
        heightPaths.push_back(layer.materialHeightPath);
    }
    result->m_textureArrays[0] = CreateTextureArray(colorPaths);
    result->m_textureArrays[1] = CreateTextureArray(normalPaths);
    result->m_textureArrays[2] = CreateTextureArray(heightPaths);
    result->m_transitionRange = layerTransitionWidth;
    return result;
}

std::unique_ptr<prev_test::render::IModel> TerrainComponentFactory::CreateModel(const std::shared_ptr<VertexData>& vertexData, const bool normalMapped) const
{
    auto mesh{ GenerateMesh(vertexData, normalMapped) };
    auto model{ prev_test::render::model::ModelFactory{ m_device }.Create(std::move(mesh)) };
    return model;
}

std::unique_ptr<prev_test::render::IMesh> TerrainComponentFactory::GenerateMesh(const std::shared_ptr<VertexData>& vertexData, const bool normalMapped) const
{
    if (normalMapped) {
        return prev_test::render::mesh::MeshFactory{}.CreateFromData(vertexData->vertices, vertexData->textureCoords, vertexData->normals, vertexData->tangents, vertexData->biTangents, vertexData->indices);
    } else {
        return prev_test::render::mesh::MeshFactory{}.CreateFromData(vertexData->vertices, vertexData->textureCoords, vertexData->normals, vertexData->indices);
    }
}

std::unique_ptr<TerrainComponentFactory::VertexData> TerrainComponentFactory::GenerateVertexData(const std::shared_ptr<HeightMapInfo>& heightMap, const float size) const
{
    const auto verticesCount{ m_vertexCount * m_vertexCount };
    auto result{ std::make_unique<VertexData>() };
    result->vertices.reserve(verticesCount);
    result->textureCoords.reserve(verticesCount);
    for (uint32_t z = 0; z < m_vertexCount; ++z) {
        for (uint32_t x = 0; x < m_vertexCount; ++x) {
            result->vertices.push_back(CalculatePosition(heightMap, x, z, size));
            result->textureCoords.push_back(CalculateTextureCoordinates(x, z));
        }
    }

    const auto indicesCount{ 6 * (m_vertexCount - 1) * (m_vertexCount - 1) };
    result->indices.reserve(indicesCount);
    for (uint32_t z = 0; z < m_vertexCount - 1; ++z) {
        for (uint32_t x = 0; x < m_vertexCount - 1; ++x) {
            const uint32_t bottomLeft{ (z * m_vertexCount) + x };
            const uint32_t bottomRight{ bottomLeft + 1 };
            const uint32_t topLeft{ ((z + 1) * m_vertexCount) + x };
            const uint32_t topRight{ topLeft + 1 };

            result->indices.push_back(topLeft);
            result->indices.push_back(topRight);
            result->indices.push_back(bottomRight);
            result->indices.push_back(bottomRight);
            result->indices.push_back(bottomLeft);
            result->indices.push_back(topLeft);
        }
    }

    result->normals = prev_test::render::mesh::MeshUtil::GenerateNormals(result->vertices, result->indices, true);

    std::tie(result->tangents, result->biTangents) = prev_test::render::mesh::MeshUtil::GenerateTangetsAndBiTangents(result->vertices, result->textureCoords, result->normals, result->indices);

    return result;
}

glm::vec3 TerrainComponentFactory::CalculatePosition(const std::shared_ptr<HeightMapInfo>& heightMap, const int x, const int z, const float size) const
{
    glm::vec3 result{};
    result.x = (static_cast<float>(x) / static_cast<float>(m_vertexCount - 1)) * size;
    result.y = heightMap->GetHeightAt(x, z);
    result.z = (static_cast<float>(z) / static_cast<float>(m_vertexCount - 1)) * size;
    return result;
}

glm::vec2 TerrainComponentFactory::CalculateTextureCoordinates(const int x, const int z) const
{
    // To match every other mesh in the engine:
    // U standardly increases with +worldX
    // V decreases with +worldZ so the derived tangent basis is right-handed (cross(T,B)==N).
    glm::vec2 result{};
    result.x = 2.0f * (static_cast<float>(x) / static_cast<float>(m_vertexCount - 1)) - 1.0f;
    result.y = 1.0f - 2.0f * (static_cast<float>(z) / static_cast<float>(m_vertexCount - 1));
    return result;
}

std::unique_ptr<HeightMapInfo> TerrainComponentFactory::CreateHeightMap(const HeightGenerator& generator) const
{
    float minHeight{ std::numeric_limits<float>::max() };
    float maxHeight{ -std::numeric_limits<float>::max() };
    auto heightMapInfo{ std::make_unique<HeightMapInfo>(m_vertexCount) };
    for (unsigned int z = 0; z < m_vertexCount; ++z) {
        for (unsigned int x = 0; x < m_vertexCount; ++x) {
            const float height{ generator.GenerateHeight(x, z) };
            heightMapInfo->heights[x][z] = height;
            minHeight = std::min(minHeight, height);
            maxHeight = std::max(maxHeight, height);
        }
    }
    heightMapInfo->minHeight = heightMapInfo->globalMinHeight = minHeight;
    heightMapInfo->maxHeight = heightMapInfo->globalMaxHeight = maxHeight;
    return heightMapInfo;
}

std::shared_ptr<prev::render::buffer::ImageBuffer> TerrainComponentFactory::CreateTextureArray(const std::vector<std::string>& paths) const
{
    prev::render::image::ImageFactory imageFactory{};

    std::vector<std::shared_ptr<prev::render::image::IImage>> images;
    uint32_t maxWidth = 0, maxHeight = 0;
    for (const auto& path : paths) {
        auto img = imageFactory.CreateImage(path);
        maxWidth = std::max(maxWidth, img->GetWidth());
        maxHeight = std::max(maxHeight, img->GetHeight());
        images.push_back(std::move(img));
    }

    // Resize images that don't match the max dimensions
    std::vector<std::shared_ptr<prev::render::image::IImage>> finalImages;
    for (const auto& img : images) {
        if (img->GetWidth() != maxWidth || img->GetHeight() != maxHeight) {
            finalImages.push_back(imageFactory.CreateResizedImage(*img, maxWidth, maxHeight));
        } else {
            finalImages.push_back(img);
        }
    }

    std::vector<const uint8_t*> layersData;
    for (const auto& img : finalImages) {
        layersData.push_back(img->GetRawDataPtr());
    }

    return prev::render::buffer::ImageBufferBuilder{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
        .SetExtent({ maxWidth, maxHeight, 1 })
        .SetFormat(prev::util::gfx::ToImageFormat(finalImages[0]->GetChannels(), finalImages[0]->GetBitDepth(), finalImages[0]->IsFloatingPoint()))
        .SetType(GFX_TEXTURE_TYPE_2D)
        .SetViewType(GFX_TEXTURE_VIEW_TYPE_2D_ARRAY)
        .SetLayerCount(static_cast<uint32_t>(paths.size()))
        .SetLayerData(layersData, static_cast<uint64_t>(finalImages[0]->GetSize()) * finalImages[0]->GetPixelSize())
        .SetMipMapEnabled(true)
        .SetUsageFlags(GFX_TEXTURE_USAGE_COPY_SRC | GFX_TEXTURE_USAGE_COPY_DST | GFX_TEXTURE_USAGE_TEXTURE_BINDING)
        .SetLayout(GFX_TEXTURE_LAYOUT_SHADER_READ_ONLY)
        .Build();
}
} // namespace prev_test::component::terrain