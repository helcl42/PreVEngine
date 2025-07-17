#include "FontMetadataFactory.h"

#include <prev/render/buffer/ImageBufferBuilder.h>
#include <prev/render/image/ImageFactory.h>

namespace prev_test::render::font {
FontMetadataFactory::FontMetadataFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator)
    : m_device{ device }
    , m_allocator{ allocator }
{
}

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

std::shared_ptr<prev::render::buffer::ImageBuffer> FontMetadataFactory::CreateImageBuffer(const std::string& textureFilePath) const
{
    const auto image{ prev::render::image::ImageFactory{}.CreateImage(textureFilePath) };
    auto imageBuffer = prev::render::buffer::ImageBufferBuilder{ m_allocator }
                           .SetExtent({ image->GetWidth(), image->GetHeight(), 1 })
                           .SetFormat(prev::util::vk::ToImageFormat(image->GetChannels(), image->GetBitDepth(), image->IsFloatingPoint()))
                           .SetType(VK_IMAGE_TYPE_2D)
                           .SetMipMapEnabled(true)
                           .SetUsageFlags(VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
                           .SetLayerData({ image->GetRawDataPtr() })
                           .SetLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
                           .Build();
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
