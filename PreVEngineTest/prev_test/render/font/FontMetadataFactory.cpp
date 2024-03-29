#include "FontMetadataFactory.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/render/buffer/image/ImageBufferFactory.h>
#include <prev/render/image/ImageFactory.h>

namespace prev_test::render::font {
std::unique_ptr<FontMetadata> FontMetadataFactory::CreateFontMetadata(const std::string& metadataFilePath, const std::string& textureFilePath, const float aspectRatio, const int desiredPadding) const
{
    FontMetadataFile metaDataFile{ metadataFilePath };

    FontMetadataState state{};
    state.desiredPadding = desiredPadding;
    state.aspectRatio = aspectRatio;
    ExtractPaddingData(metaDataFile, state);
    ExtractMeasureInfo(metaDataFile, state);

    std::map<int, Character> characterMetaData{};
    ExtractCharactersData(metaDataFile, state, characterMetaData);

    auto metaData{ std::make_unique<FontMetadata>() };
    metaData->m_imageBuffer = CreateImageBuffer(textureFilePath);
    metaData->m_sampler = CreateSampler(static_cast<float>(metaData->m_imageBuffer->GetMipLevels()));
    metaData->m_spaceWidth = state.spaceWidth;
    metaData->m_characterMetaData = characterMetaData;

    return metaData;
}

void FontMetadataFactory::ExtractPaddingData(FontMetadataFile& metaDataFile, FontMetadataState& state) const
{
    metaDataFile.MoveToNextLine();

    state.padding = metaDataFile.GetValueAsInts("padding");
    state.paddingWidth = state.padding[PADDING_LEFT_INDEX] + state.padding[PADDING_RIGHT_INDEX];
    state.paddingHeight = state.padding[PADDING_TOP_INDEX] + state.padding[PADDING_BOTTOM_INDEX];
}

void FontMetadataFactory::ExtractMeasureInfo(FontMetadataFile& metaDataFile, FontMetadataState& state) const
{
    metaDataFile.MoveToNextLine();

    int lineHeightPixels = metaDataFile.GetValueAsInt("lineHeight") - state.paddingHeight;
    state.verticalPerPixelSize = FontMetadata::LINE_HEIGHT / (float)lineHeightPixels;
    state.horizontalPerPixelSize = state.verticalPerPixelSize / state.aspectRatio;

    state.imageWidth = metaDataFile.GetValueAsInt("scaleW");
    state.imageHeight = metaDataFile.GetValueAsInt("scaleH");
}

std::unique_ptr<prev::render::sampler::Sampler> FontMetadataFactory::CreateSampler(const float maxLod) const
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };

    return std::make_unique<prev::render::sampler::Sampler>(*device, maxLod, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR);
}

std::shared_ptr<prev::render::buffer::image::IImageBuffer> FontMetadataFactory::CreateImageBuffer(const std::string& textureFilePath) const
{
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    const auto image{ prev::render::image::ImageFactory{}.CreateImage(textureFilePath) };
    auto imageBuffer{ prev::render::buffer::image::ImageBufferFactory{}.CreateFromData(prev::render::buffer::image::ImageBufferCreateInfo{ VkExtent2D{ image->GetWidth(), image->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT, 0, true, VK_IMAGE_VIEW_TYPE_2D, 1, reinterpret_cast<uint8_t*>(image->GetBuffer()) }, *allocator) };
    return imageBuffer;
}

void FontMetadataFactory::ExtractCharactersData(FontMetadataFile& metaDataFile, FontMetadataState& state, std::map<int, Character>& characters) const
{
    metaDataFile.MoveToNextLine();
    metaDataFile.MoveToNextLine();

    int charactersCount = metaDataFile.GetValueAsInt("count");
    for (int i = 0; i < charactersCount; i++) {
        metaDataFile.MoveToNextLine();

        int charCode = metaDataFile.GetValueAsInt("id");
        if (charCode == FontMetadata::SPACE_CODE || charCode == FontMetadata::NULL_CODE) {
            state.spaceWidth = (metaDataFile.GetValueAsInt("xadvance") - state.paddingWidth) * state.horizontalPerPixelSize * 0.4f;
        } else {
            auto c = CreateSingleCharacter(metaDataFile, state, charCode);
            characters.insert({ c.GetId(), c });
        }
    }
}

Character FontMetadataFactory::CreateSingleCharacter(FontMetadataFile& metaDataFile, const FontMetadataState& state, const int charCode) const
{
    const int width{ metaDataFile.GetValueAsInt("width") - (state.paddingWidth - (2 * state.desiredPadding)) };
    const int height{ metaDataFile.GetValueAsInt("height") - ((state.paddingHeight) - (2 * state.desiredPadding)) };

    const glm::vec2 textureCoord{ (static_cast<float>(metaDataFile.GetValueAsInt("x")) + (state.padding[PADDING_LEFT_INDEX] - state.desiredPadding)) / state.imageWidth, (static_cast<float>(metaDataFile.GetValueAsInt("y")) + (state.padding[PADDING_TOP_INDEX] - state.desiredPadding)) / state.imageWidth };
    const glm::vec2 textureSize{ static_cast<float>(width) / state.imageWidth, static_cast<float>(height) / state.imageWidth };
    const glm::vec2 offset{ (metaDataFile.GetValueAsInt("xoffset") + state.padding[PADDING_LEFT_INDEX] - state.desiredPadding) * state.horizontalPerPixelSize, (metaDataFile.GetValueAsInt("yoffset") + (state.padding[PADDING_TOP_INDEX] - state.desiredPadding)) * state.verticalPerPixelSize };
    const glm::vec2 quadSize{ width * state.horizontalPerPixelSize, height * state.verticalPerPixelSize };
    const float xAdvance = (metaDataFile.GetValueAsInt("xadvance") - state.paddingWidth) * state.horizontalPerPixelSize;
    return Character(charCode, textureCoord, textureSize, offset, quadSize, xAdvance);
}

} // namespace prev_test::render::font
