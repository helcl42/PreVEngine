#ifndef __FONT_H__
#define __FONT_H__

#include <Buffers.h>
#include <Common.h>
#include <Image.h>

#include "General.h"

#include <fstream>
#include <iostream>
#include <map>

class Character;
class Word;
class TextLine;
class TextMeshData;
class TextMeshCreator;
class BasicText;
class TextMaster;
class FontMetadata;

class Character {
private:
    int m_id;

    glm::vec2 m_textureCoords;

    glm::vec2 m_maxTextureCoords;

    glm::vec2 m_offset;

    glm::vec2 m_size;

    float m_xAdvance;

public:
    explicit Character() = default;

    explicit Character(int id, const glm::vec2& textureCoord, const glm::vec2& texSize, const glm::vec2& offset, const glm::vec2& size, float xAdvance)
        : m_id(id)
        , m_textureCoords(textureCoord)
        , m_maxTextureCoords(texSize + textureCoord)
        , m_offset(offset)
        , m_size(size)
        , m_xAdvance(xAdvance)
    {
    }

    ~Character() = default;

public:
    int GetId() const
    {
        return m_id;
    }

    glm::vec2 GetTextureCoords() const
    {
        return m_textureCoords;
    }

    glm::vec2 GetMaxTextureCoords() const
    {
        return m_maxTextureCoords;
    }

    glm::vec2 GetOffset() const
    {
        return m_offset;
    }

    glm::vec2 GetSize() const
    {
        return m_size;
    }

    float GetXAdvance() const
    {
        return m_xAdvance;
    }
};

class FontMetadataFactory;

class FontMetadata {
private:
    friend class FontMetadataFactory;

private:
    float m_spaceWidth;

    std::map<int, Character> m_characterMetaData;

    std::shared_ptr<Image> m_image;

    std::shared_ptr<ImageBuffer> m_imageBuffer;

public:
    static const float LINE_HEIGHT;

    static const int NULL_CODE = 0;

    static const int SPACE_CODE = ' ';

    static const int NEW_LINE_CODE = '\n';

    static const int FALLBACK_CODE = '?';

public:
    explicit FontMetadata() {}

    ~FontMetadata() {}

public:
    float GetSpaceWidth() const
    {
        return m_spaceWidth;
    }

    std::shared_ptr<Image> GetImage() const
    {
        return m_image;
    }

    std::shared_ptr<ImageBuffer> GetImageBuffer() const
    {
        return m_imageBuffer;
    }

    bool GetCharacter(const int charCode, Character& outCharacter) const
    {
        if (m_characterMetaData.find(charCode) != m_characterMetaData.cend()) {
            outCharacter = m_characterMetaData.at(charCode);
            return true;
        }
        return false;
    }
};

const float FontMetadata::LINE_HEIGHT = 0.03f;

class MetaDataFile {
public:
    MetaDataFile(const std::string& path)
    {
        std::ifstream fileStram{ path };
        if (!fileStram.is_open()) {
            throw std::runtime_error("Could not open metadata file: " + path);
        }

        m_allLinesKeyValues = GetAllLinesTokens(fileStram);
        if (m_allLinesKeyValues.size() > 0) {
            m_currentLine = m_allLinesKeyValues.at(0);
        }
    }

    ~MetaDataFile() = default;

private:
    bool ProcessNextLine(std::ifstream& inOutFileStream, std::map<std::string, std::string>& outTokens) const
    {
        outTokens.clear();

        std::string line;
        std::getline(inOutFileStream, line);

        if (line.empty()) {
            return false;
        }

        auto items = StringUtils::Split(line, ' ');
        for (auto& part : items) {
            auto valuePairs = StringUtils::Split(part, '=');
            if (valuePairs.size() == 2) {
                outTokens.insert({ valuePairs[0], valuePairs[1] });
            }
        }
        return true;
    }

    std::vector<std::map<std::string, std::string> > GetAllLinesTokens(std::ifstream& inOutFileStream) const
    {
        std::vector<std::map<std::string, std::string> > result{};

        std::map<std::string, std::string> lineTokens{};
        while (ProcessNextLine(inOutFileStream, lineTokens)) {
            result.emplace_back(lineTokens);
        }

        return result;
    }

public:
    void MoveToNextLine()
    {
        m_currentLine = m_allLinesKeyValues.at(m_lineNumber++);
    }

    bool ValuesContains(const std::string& variable) const
    {
        return m_currentLine.find(variable) != m_currentLine.cend();
    }

    std::string GetValueAsString(const std::string& variable) const
    {
        return m_currentLine.at(variable);
    }

    int GetValueAsInt(const std::string& variable) const
    {
        return std::stoi(m_currentLine.at(variable));
    }

    std::vector<int> GetValueAsInts(const std::string& variable) const
    {
        std::vector<int> actualValues;
        auto numberStrings = StringUtils::Split(m_currentLine.at(variable), ',');
        for (size_t i = 0; i < numberStrings.size(); i++) {
            actualValues.push_back(std::stoi(numberStrings[i]));
        }
        return actualValues;
    }

private:
    std::vector<std::map<std::string, std::string> > m_allLinesKeyValues;

    std::map<std::string, std::string> m_currentLine;

    uint32_t m_lineNumber = 0;
};

class FontMetadataFactory {
private:
    static const int PADDING_TOP_INDEX = 0;
    static const int PADDING_LEFT_INDEX = 1;
    static const int PADDING_BOTTOM_INDEX = 2;
    static const int PADDING_RIGHT_INDEX = 3;    

    struct FontMetaDataState {
        float aspectRatio;

        float verticalPerPixelSize;

        float horizontalPerPixelSize;

        std::vector<int> padding;

        int paddingWidth;

        int paddingHeight;

        float spaceWidth;

        int imageWidth;

        int imageHeight;

        int desiredPadding;
    };

private:
    void ExtractPaddingData(MetaDataFile& metaDataFile, FontMetaDataState& state) const
    {
        metaDataFile.MoveToNextLine();

        state.padding = metaDataFile.GetValueAsInts("padding");
        state.paddingWidth = state.padding[PADDING_LEFT_INDEX] + state.padding[PADDING_RIGHT_INDEX];
        state.paddingHeight = state.padding[PADDING_TOP_INDEX] + state.padding[PADDING_BOTTOM_INDEX];
    }

    void ExtractMeasureInfo(MetaDataFile& metaDataFile, FontMetaDataState& state) const
    {
        metaDataFile.MoveToNextLine();

        int lineHeightPixels = metaDataFile.GetValueAsInt("lineHeight") - state.paddingHeight;
        state.verticalPerPixelSize = FontMetadata::LINE_HEIGHT / (float)lineHeightPixels;
        state.horizontalPerPixelSize = state.verticalPerPixelSize / state.aspectRatio;

        state.imageWidth = metaDataFile.GetValueAsInt("scaleW");
        state.imageHeight = metaDataFile.GetValueAsInt("scaleH");
    }

    void ExtractImageFIle(MetaDataFile& metaDataFile, const std::string& metadataFilePath, std::shared_ptr<Image>& image, std::shared_ptr<ImageBuffer>& imageBuffer) const
    {
        metaDataFile.MoveToNextLine();

        const auto imagePath = StringUtils::Replace(metaDataFile.GetValueAsString("file"), "\"", "");
        ImageFactory imageFactory;
        image = imageFactory.CreateImage(imagePath);
        imageBuffer = std::make_unique<ImageBuffer>(*AllocatorProvider::GetInstance().GetAllocator());
        imageBuffer->Create(ImageBufferCreateInfo{ { image->GetWidth(), image->GetHeight() }, VK_FORMAT_R8G8B8A8_UNORM, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, image->GetBuffer() });
    }

    void ExtractCharactersData(MetaDataFile& metaDataFile, FontMetaDataState& state, std::map <int, Character>& characters) const
    {
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

    Character CreateSingleCharacter(MetaDataFile& metaDataFile, const FontMetaDataState& state, const int charCode) const
    {
        float xTex = ((float)metaDataFile.GetValueAsInt("x") + (state.padding[PADDING_LEFT_INDEX] - state.desiredPadding)) / state.imageWidth;
        float yTex = ((float)metaDataFile.GetValueAsInt("y") + (state.padding[PADDING_TOP_INDEX] - state.desiredPadding)) / state.imageWidth;
        int width = metaDataFile.GetValueAsInt("width") - (state.paddingWidth - (2 * state.desiredPadding));
        int height = metaDataFile.GetValueAsInt("height") - ((state.paddingHeight) - (2 * state.desiredPadding));
        float quadWidth = width * state.horizontalPerPixelSize;
        float quadHeight = height * state.verticalPerPixelSize;
        float xTexSize = (float)width / state.imageWidth;
        float yTexSize = (float)height / state.imageWidth;
        float xOff = (metaDataFile.GetValueAsInt("xoffset") + state.padding[PADDING_LEFT_INDEX] - state.desiredPadding) * state.horizontalPerPixelSize;
        float yOff = (metaDataFile.GetValueAsInt("yoffset") + (state.padding[PADDING_TOP_INDEX] - state.desiredPadding)) * state.verticalPerPixelSize;
        float xAdvance = (metaDataFile.GetValueAsInt("xadvance") - state.paddingWidth) * state.horizontalPerPixelSize;
        return Character(charCode, glm::vec2(xTex, yTex), glm::vec2(xTexSize, yTexSize), glm::vec2(xOff, yOff), glm::vec2(quadWidth, quadHeight), xAdvance);
    }

public:
    std::unique_ptr<FontMetadata> CreateFontMetadata(const std::string& metadataFilePath, int desiredPadding = 0, float aspectRatio = 16.0f / 9.0f)
    {
        MetaDataFile metaDataFile{ metadataFilePath };

        std::map<int, Character> characterMetaData{};

        FontMetaDataState state{};
        state.desiredPadding = desiredPadding;
        state.aspectRatio = aspectRatio;
        ExtractPaddingData(metaDataFile, state);
        ExtractMeasureInfo(metaDataFile, state);

        std::shared_ptr<Image> image;
        std::shared_ptr<ImageBuffer> imagwBuffer;
        ExtractImageFIle(metaDataFile, metadataFilePath, image, imagwBuffer);

        ExtractCharactersData(metaDataFile, state, characterMetaData);

        std::unique_ptr<FontMetadata> metaData = std::make_unique<FontMetadata>();
        metaData->m_image = image;
        metaData->m_imageBuffer = imagwBuffer;
        metaData->m_spaceWidth = state.spaceWidth;
        metaData->m_characterMetaData = characterMetaData;

        return metaData;
    }
};

#endif // !__FONT_H__
