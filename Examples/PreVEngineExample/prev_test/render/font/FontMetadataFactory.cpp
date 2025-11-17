#include "FontMetadataFactory.h"

#include <prev/render/buffer/ImageBufferBuilder.h>
#include <prev/render/image/ImageFactory.h>

#include <prev/util/Utils.h>

namespace prev_test::render::font {
namespace {
    std::string GetValueAsString(const std::map<std::string, std::string>& keyValuePairs, const std::string& variable)
    {
        return keyValuePairs.at(variable);
    }

    int GetValueAsInt(const std::map<std::string, std::string>& keyValuePairs, const std::string& variable)
    {
        return std::stoi(GetValueAsString(keyValuePairs, variable));
    }

    std::vector<int> GetValueAsInts(const std::map<std::string, std::string>& keyValuePairs, const std::string& variable)
    {
        const auto numberStrings{ prev::util::string::Split(GetValueAsString(keyValuePairs, variable), ',') };

        std::vector<int> actualValues(numberStrings.size());
        for (size_t i = 0; i < numberStrings.size(); ++i) {
            actualValues[i] = std::stoi(numberStrings[i]);
        }
        return actualValues;
    }

    std::map<std::string, std::string> GetLineTokens(const std::string& line)
    {
        std::map<std::string, std::string> tokens;
        const auto items{ prev::util::string::Split(line, ' ') };
        for (const auto& part : items) {
            const auto valuePairs{ prev::util::string::Split(part, '=') };
            if (valuePairs.size() != 2) {
                continue;
            }
            tokens.insert({ valuePairs[0], valuePairs[1] });
        }
        return tokens;
    }

    std::vector<std::map<std::string, std::string>> GetAllLinesTokens(const std::vector<std::string>& lines)
    {
        std::vector<std::map<std::string, std::string>> allLinesTokens;
        for (const auto& line : lines) {
            if (line.empty()) {
                continue;
            }
            const auto lineTokens{ GetLineTokens(line) };
            allLinesTokens.push_back(lineTokens);
        }
        return allLinesTokens;
    }

    struct Padding {
        float top{};
        float left{};
        float bottom{};
        float right{};

        float GetWidth() const
        {
            return left + right;
        }

        float GetHeight() const
        {
            return top + bottom;
        }
    };

    struct CharacterMetadataState {
        int id{};

        glm::vec2 size{};

        glm::vec2 position{};

        glm::vec2 offset{};

        glm::vec2 advance{};
    };

    struct FontMetadataState {
        float lineHeight{};

        Padding padding{};

        glm::uvec2 imageSize{};

        std::vector<CharacterMetadataState> characters{};
    };

    FontMetadataState ParseFontMetadataFile(const std::string& metadataFilePath)
    {
        const std::string allText{ prev::util::file::ReadTextFile(metadataFilePath) };
        const std::vector<std::string> allLines{ prev::util::string::Split(allText, '\n') };
        const std::vector<std::map<std::string, std::string>> allLinesKeyValues{ GetAllLinesTokens(allLines) };

        FontMetadataState state{};

        uint32_t lineIndex{ 0 };

        {
            const auto& lineTokens{ allLinesKeyValues[lineIndex++] };

            const auto paddingValues{ GetValueAsInts(lineTokens, "padding") };
            state.padding.top = static_cast<float>(paddingValues[0]);
            state.padding.left = static_cast<float>(paddingValues[1]);
            state.padding.bottom = static_cast<float>(paddingValues[2]);
            state.padding.right = static_cast<float>(paddingValues[3]);
        }

        {
            const auto& lineTokens{ allLinesKeyValues[lineIndex++] };

            state.lineHeight = static_cast<float>(GetValueAsInt(lineTokens, "lineHeight"));
            state.imageSize = { static_cast<unsigned int>(GetValueAsInt(lineTokens, "scaleW")), static_cast<unsigned int>(GetValueAsInt(lineTokens, "scaleH")) };
        }

        lineIndex++;

        const auto charCount{ static_cast<uint32_t>(GetValueAsInt(allLinesKeyValues[lineIndex++], "count")) };
        for (uint32_t i = 0; i < charCount; ++i) {
            const auto& lineTokens{ allLinesKeyValues[lineIndex++] };

            CharacterMetadataState charMeta{};
            charMeta.id = GetValueAsInt(lineTokens, "id");
            charMeta.size = glm::vec2{ static_cast<float>(GetValueAsInt(lineTokens, "width")), static_cast<float>(GetValueAsInt(lineTokens, "height")) };
            charMeta.position = glm::vec2{ static_cast<float>(GetValueAsInt(lineTokens, "x")), static_cast<float>(GetValueAsInt(lineTokens, "y")) };
            charMeta.offset = glm::vec2{ static_cast<float>(GetValueAsInt(lineTokens, "xoffset")), static_cast<float>(GetValueAsInt(lineTokens, "yoffset")) };
            charMeta.advance = glm::vec2{ static_cast<float>(GetValueAsInt(lineTokens, "xadvance")), 0.0f };
            state.characters.push_back(charMeta);
        }

        return state;
    }

    glm::vec2 FindPerPixelSizes(const FontMetadataState& state, const float aspectRatio, const float lineHeight)
    {
        const float verticalPerPixelSize{ lineHeight / static_cast<float>(state.lineHeight - state.padding.GetHeight()) };
        const float horizontalPerPixelSize{ verticalPerPixelSize / aspectRatio };
        return glm::vec2{ horizontalPerPixelSize, verticalPerPixelSize };
    }

    float FindSpaceWidth(const FontMetadataState& state, const glm::vec2& perPixelSize)
    {
        for (const auto& charMeta : state.characters) {
            if (charMeta.id == FontMetadata::SPACE_CHARACTER || charMeta.id == FontMetadata::NULL_CHARACTER) {
                return (charMeta.advance.x - state.padding.GetWidth()) * perPixelSize.x * 0.4f; // TODO: Magic Constant
            }
        }
        LOGW("Space character not found in font metadata!");
        return 0.0f;
    }

    Character CreateCharacter(const FontMetadataState& state, const glm::vec2& perPixelSize, const int extraPadding, const CharacterMetadataState& charMeta)
    {
        const glm::vec2 charSize{ charMeta.size.x - (state.padding.GetWidth() - (2 * extraPadding)), charMeta.size.y - (state.padding.GetHeight() - (2 * extraPadding)) };
        const glm::vec2 imageSize{ static_cast<float>(state.imageSize.x), static_cast<float>(state.imageSize.y) };

        const glm::vec2 textureCoord{ (charMeta.position.x + state.padding.left - extraPadding) / imageSize.x, (charMeta.position.y + state.padding.top - extraPadding) / imageSize.y };
        const glm::vec2 textureSize{ charSize.x / imageSize.x, charSize.y / imageSize.y };
        const glm::vec2 offset{ (charMeta.offset.x + state.padding.left - extraPadding) * perPixelSize.x, (charMeta.offset.y + state.padding.top - extraPadding) * perPixelSize.y };
        const glm::vec2 quadSize{ charSize.x * perPixelSize.x, charSize.y * perPixelSize.y };
        const float xAdvance{ (charMeta.advance.x - state.padding.GetWidth()) * perPixelSize.x };
        return Character(charMeta.id, textureCoord, textureSize, offset, quadSize, xAdvance);
    }

    std::map<int, Character> CreateCharacters(const FontMetadataState& state, const glm::vec2& perPixelSize, const int extraPadding)
    {
        std::map<int, Character> characters;
        for (const auto& charMeta : state.characters) {
            if (charMeta.id == FontMetadata::SPACE_CHARACTER || charMeta.id == FontMetadata::NULL_CHARACTER) {
                continue;
            }
            const auto c{ CreateCharacter(state, perPixelSize, extraPadding, charMeta) };
            characters.insert({ c.GetId(), c });
        }
        return characters;
    }

    std::shared_ptr<prev::render::buffer::ImageBuffer> CreateImageBuffer(prev::core::memory::Allocator& allocator, const std::string& textureFilePath)
    {
        const auto image{ prev::render::image::ImageFactory{}.CreateImage(textureFilePath) };
        auto imageBuffer = prev::render::buffer::ImageBufferBuilder{ allocator }
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
} // namespace

FontMetadataFactory::FontMetadataFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator)
    : m_device{ device }
    , m_allocator{ allocator }
{
}

std::unique_ptr<FontMetadata> FontMetadataFactory::CreateFontMetadata(const std::string& metadataFilePath, const std::string& textureFilePath, const float aspectRatio, const float lineHeight, const int extraPadding) const
{
    const FontMetadataState metaDataState{ ParseFontMetadataFile(metadataFilePath) };
    const glm::vec2 perPixelSize{ FindPerPixelSizes(metaDataState, aspectRatio, lineHeight) };

    const std::shared_ptr<prev::render::buffer::ImageBuffer> imageBuffer{ CreateImageBuffer(m_allocator, textureFilePath) };
    const std::map<int, Character> characters{ CreateCharacters(metaDataState, perPixelSize, extraPadding) };
    const float spaceWidth{ FindSpaceWidth(metaDataState, perPixelSize) };

    auto metaData{ std::make_unique<FontMetadata>(imageBuffer, characters, spaceWidth, lineHeight) };
    return metaData;
}
} // namespace prev_test::render::font
