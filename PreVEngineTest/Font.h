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

    void ExtractCharactersData(MetaDataFile& metaDataFile, FontMetaDataState& state, std::map<int, Character>& characters) const
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
    std::unique_ptr<FontMetadata> CreateFontMetadata(const std::string& metadataFilePath, const int desiredPadding = 0, const float aspectRatio = 16.0f / 9.0f) const
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

class Word {
private:
    std::vector<Character> m_characters;

    float m_width = 0;

    float m_fontSize;

public:
    explicit Word(const float fontSize)
        : m_fontSize(fontSize)
    {
    }

    virtual ~Word()
    {
    }

public:
    void AddCharacter(const Character& character)
    {
        m_characters.push_back(character);
        m_width += character.GetXAdvance() * m_fontSize;
    }

    const std::vector<Character>& GetCharacters() const
    {
        return m_characters;
    }

    float GetWordWidth() const
    {
        return m_width;
    }

    float GetFontSize() const
    {
        return m_fontSize;
    }
};

class TextLine {
private:
    float m_maxLength;

    float m_spaceSize;

    std::vector<Word> m_words;

    float m_currentLength = 0;

public:
    explicit TextLine(float spaceWidth, float fontSize, float maxLength)
        : m_maxLength(maxLength)
        , m_spaceSize(spaceWidth * fontSize)
    {
    }

    ~TextLine() = default;

public:
    bool AttemptToAddWord(const Word& word)
    {
        float additionalLength = word.GetWordWidth();
        additionalLength += !m_words.empty() ? m_spaceSize : 0;
        if (m_currentLength + additionalLength <= m_maxLength) {
            m_words.push_back(word);
            m_currentLength += additionalLength;
            return true;
        }
        return false;
    }

    float GetMaxLength() const
    {
        return m_maxLength;
    }

    float GetCurrentLength() const
    {
        return m_currentLength;
    }

    const std::vector<Word>& GetWords() const
    {
        return m_words;
    }
};

class BaseFancyText {
protected:
    std::shared_ptr<FontMetadata> m_font;

    std::wstring m_textString;

    float m_fontSize;

    glm::vec4 m_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    float m_lineMaxSize;

    unsigned int m_numberOfLines;

    bool m_centerText = false;

    float m_width;

    float m_edge;

    bool m_hasEffect;

    float m_borderWidth;

    float m_borderEdge;

    glm::vec3 m_outlineColor;

    glm::vec2 m_outlineOffset;

public:
    explicit BaseFancyText(const std::shared_ptr<FontMetadata>& font, const std::wstring& text, float fontSize, const glm::vec4& color, float maxLineLength, bool centered, float width, float edge)
        : m_font(font)
        , m_textString(text)
        , m_fontSize(fontSize)
        , m_color(color)
        , m_lineMaxSize(maxLineLength)
        , m_centerText(centered)
        , m_width(width)
        , m_edge(edge)
        , m_hasEffect(false)
    {
    }

    explicit BaseFancyText(const std::shared_ptr<FontMetadata>& font, const std::string& text, float fontSize, const glm::vec4& color, float maxLineLength, bool centered, float width, float edge)
        : m_font(font)
        , m_fontSize(fontSize)
        , m_color(color)
        , m_lineMaxSize(maxLineLength)
        , m_centerText(centered)
        , m_width(width)
        , m_edge(edge)
        , m_hasEffect(false)
    {
        m_textString.assign(text.begin(), text.end());
    }

    virtual ~BaseFancyText() = default;

public:
    void SetColor(const glm::vec4& color)
    {
        m_color = color;
    }

    const glm::vec4& GetColor() const
    {
        return m_color;
    }

    int GetNumberOfLines() const
    {
        return m_numberOfLines;
    }

    float GetFontSize() const
    {
        return m_fontSize;
    }

    std::shared_ptr<FontMetadata> GetFont() const
    {
        return m_font;
    }

    void SetNumberOfLines(unsigned int number)
    {
        m_numberOfLines = number;
    }

    bool IsCentered() const
    {
        return m_centerText;
    }

    float GetMaxLineSize() const
    {
        return m_lineMaxSize;
    }

    std::wstring GetTextString() const
    {
        return m_textString;
    }

    float GetWidth() const
    {
        return m_width;
    }

    float GetEdge() const
    {
        return m_edge;
    }

    bool HasEffect() const
    {
        return m_hasEffect;
    }

    float GetBorderWidth() const
    {
        return m_borderWidth;
    }

    float GetBorderEdge() const
    {
        return m_borderEdge;
    }

    glm::vec3 GetOutlineColor() const
    {
        return m_outlineColor;
    }

    glm::vec2 GetOutlineOffset() const
    {
        return m_outlineOffset;
    }

    void EnableEffect(float borderWidth, float borderEdge, const glm::vec3& outlineColor, const glm::vec2& outlineOffset)
    {
        m_hasEffect = true;
        m_borderWidth = borderWidth;
        m_borderEdge = borderEdge;
        m_outlineColor = outlineColor;
        m_outlineOffset = outlineOffset;
    }

    void DisableEffect()
    {
        m_hasEffect = false;
    }
};

class FancyText : public BaseFancyText {
private:
    glm::vec2 m_position;

public:
    explicit FancyText(const std::shared_ptr<FontMetadata>& font, const std::wstring& text, float fontSize, const glm::vec4& color, const glm::vec2& position, float maxLineLength, bool centered, float width, float edge)
        : BaseFancyText(font, text, fontSize, color, maxLineLength, centered, width, edge)
        , m_position(position)
    {
    }

    explicit FancyText(const std::shared_ptr<FontMetadata>& font, const std::string& text, float fontSize, const glm::vec4& color, const glm::vec2& position, float maxLineLength, bool centered, float width, float edge)
        : BaseFancyText(font, text, fontSize, color, maxLineLength, centered, width, edge)
        , m_position(position)
    {
    }

    virtual ~FancyText() = default;

public:
    const glm::vec2& GetPosition() const
    {
        return m_position;
    }
};

class FancyTextInSpace : public BaseFancyText {
private:
    glm::vec3 m_position;

    glm::vec3 m_orientation;

    bool m_ignoreOverallRotations = false;

public:
    explicit FancyTextInSpace(const std::shared_ptr<FontMetadata>& font, const std::wstring& text, float fontSize, const glm::vec4& color, const glm::vec3& position, const glm::vec3& orientation, bool ignoreOverallRotations, float maxLineLength, bool centered, float width, float edge)
        : BaseFancyText(font, text, fontSize, color, maxLineLength, centered, width, edge)
        , m_position(position)
        , m_orientation(orientation)
        , m_ignoreOverallRotations(ignoreOverallRotations)
    {
    }

    explicit FancyTextInSpace(const std::shared_ptr<FontMetadata>& font, const std::string& text, float fontSize, const glm::vec4& color, const glm::vec3& position, const glm::vec3& orientation, bool ignoreOverallRotations, float maxLineLength, bool centered, float width, float edge)
        : BaseFancyText(font, text, fontSize, color, maxLineLength, centered, width, edge)
        , m_position(position)
        , m_orientation(orientation)
        , m_ignoreOverallRotations(ignoreOverallRotations)
    {
    }

    virtual ~FancyTextInSpace() = default;

public:
    const glm::vec3& GetPosition() const
    {
        return m_position;
    }

    const glm::vec3& GetOrientation() const
    {
        return m_orientation;
    }

    bool IsCameraRotationIgnored() const
    {
        return m_ignoreOverallRotations;
    }
};

class TextMesh final : public IMesh {
private:
    VertexLayout m_vertexLayout;

    uint32_t m_verticesCount;

    std::vector<uint32_t> m_indices;

    VertexDataBuffer m_vertexDataBuffer;

public:
    TextMesh(const std::vector<glm::vec2>& vertices, const std::vector<glm::vec2>& textureCoords, const std::vector<uint32_t>& indices)
        : m_vertexLayout({ { VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC2 } })
        , m_verticesCount(static_cast<uint32_t>(vertices.size()))
        , m_indices(indices)
    {
        for (size_t i = 0; i < vertices.size(); i++) {
            m_vertexDataBuffer.Add(&vertices[i], sizeof(glm::vec2));
            m_vertexDataBuffer.Add(&textureCoords[i], sizeof(glm::vec2));
        }
    }

    ~TextMesh() = default;

public:
    const VertexLayout& GetVertextLayout() const override
    {
        return m_vertexLayout;
    }

    const void* GetVertices() const override
    {
        return m_vertexDataBuffer.GetData();
    }

    uint32_t GerVerticesCount() const override
    {
        return m_verticesCount;
    }

    const std::vector<uint32_t>& GerIndices() const override
    {
        return m_indices;
    }

    bool HasIndices() const override
    {
        return m_indices.size() > 0;
    }
};

class TextMeshFactory {
public:
    std::unique_ptr<IMesh> CreateTextMesh(const std::shared_ptr<BaseFancyText>& text, const std::shared_ptr<FontMetadata> fontMetaData) const
    {
        std::vector<TextLine> lines;
        CreateStructure(text, fontMetaData, lines);
        return CreateQuadVertices(text, fontMetaData, lines);
    }

private:
    void CreateStructure(const std::shared_ptr<BaseFancyText>& text, const std::shared_ptr<FontMetadata> fontMetaData, std::vector<TextLine>& lines) const
    {
        TextLine currentLine{ fontMetaData->GetSpaceWidth(), text->GetFontSize(), text->GetMaxLineSize() };
        Word currentWord{ text->GetFontSize() };
        for (auto c : text->GetTextString()) {
            int charCode = (int)c;
            if (charCode == FontMetadata::SPACE_CODE) {
                bool added = currentLine.AttemptToAddWord(currentWord);
                if (!added) {
                    lines.push_back(currentLine);
                    currentLine = TextLine{ fontMetaData->GetSpaceWidth(), text->GetFontSize(), text->GetMaxLineSize() };
                    currentLine.AttemptToAddWord(currentWord);
                }
                currentWord = Word{ text->GetFontSize() };
                continue;
            } else if (charCode == FontMetadata::NEW_LINE_CODE) {
                bool added = currentLine.AttemptToAddWord(currentWord);
                lines.push_back(currentLine);
                currentLine = TextLine{ fontMetaData->GetSpaceWidth(), text->GetFontSize(), text->GetMaxLineSize() };
                if (!added) {
                    currentLine.AttemptToAddWord(currentWord);
                }
                currentWord = Word{ text->GetFontSize() };
                continue;
            }

            Character character{};
            if (!fontMetaData->GetCharacter(charCode, character)) {
                fontMetaData->GetCharacter(FontMetadata::FALLBACK_CODE, character);
            }
            currentWord.AddCharacter(character);
        }
        CompleteStructure(text, fontMetaData, lines, currentLine, currentWord);
    }

    void CompleteStructure(const std::shared_ptr<BaseFancyText>& text, const std::shared_ptr<FontMetadata> fontMetaData, std::vector<TextLine>& lines, TextLine& currentLine, Word& currentWord) const
    {
        bool added = currentLine.AttemptToAddWord(currentWord);
        if (!added) {
            lines.push_back(currentLine);
            currentLine = TextLine{ fontMetaData->GetSpaceWidth(), text->GetFontSize(), text->GetMaxLineSize() };
            currentLine.AttemptToAddWord(currentWord);
        }
        lines.push_back(currentLine);
    }

    std::unique_ptr<IMesh> CreateQuadVertices(const std::shared_ptr<BaseFancyText>& text, const std::shared_ptr<FontMetadata> fontMetaData, const std::vector<TextLine>& lines) const
    {
        text->SetNumberOfLines(static_cast<unsigned int>(lines.size()));

        float curserX = 0.0f;
        float curserY = 0.0f;
        std::vector<glm::vec2> vertices;
        std::vector<glm::vec2> textureCoords;
        std::vector<uint32_t> indices;
        for (auto line : lines) {
            if (text->IsCentered()) {
                curserX = (line.GetMaxLength() - line.GetCurrentLength()) / 2.0f;
            }

            for (auto word : line.GetWords()) {
                for (auto letter : word.GetCharacters()) {
                    AddVerticesForCharacter(curserX, curserY, letter, text->GetFontSize(), vertices);
                    AddQuadData(letter.GetTextureCoords().x, letter.GetTextureCoords().y, letter.GetMaxTextureCoords().x, letter.GetMaxTextureCoords().y, textureCoords);
                    AddIndices(indices);
                    curserX += letter.GetXAdvance() * text->GetFontSize();
                }
                curserX += fontMetaData->GetSpaceWidth() * text->GetFontSize();
            }
            curserX = 0;
            curserY += FontMetadata::LINE_HEIGHT * text->GetFontSize();
        }
        return std::make_unique<TextMesh>(vertices, textureCoords, indices);
    }

    void AddVerticesForCharacter(const float curserX, const float curserY, const Character& character, const float fontSize, std::vector<glm::vec2>& vertices) const
    {
        const float x = curserX + (character.GetOffset().x * fontSize);
        const float y = curserY + (character.GetOffset().y * fontSize);
        const float maxX = x + (character.GetSize().x * fontSize);
        const float maxY = y + (character.GetSize().y * fontSize);
        const float properX = (2 * x) - 1;
        const float properY = (-2 * y) + 1;
        const float properMaxX = (2 * maxX) - 1;
        const float properMaxY = (-2 * maxY) + 1;

        AddQuadData(properX, properY, properMaxX, properMaxY, vertices);
    }

    void AddQuadData(const float x, const float y, const float maxX, const float maxY, std::vector<glm::vec2>& vertices) const
    {
        const glm::vec2 planeVertices[] = {
            glm::vec2{ x, y },
            glm::vec2{ maxX, y },
            glm::vec2{ maxX, maxY },
            glm::vec2{ x, maxY }
        };

        for (uint32_t i = 0; i < 4; i++) {
            vertices.push_back(planeVertices[i]);
        }
    }

    void AddIndices(std::vector<uint32_t>& inOutIndices) const
    {
        const uint32_t indices[] = { 0, 3, 2, 2, 1, 0 };
        const uint32_t baseIndex = static_cast<uint32_t>(inOutIndices.size());
        for (uint32_t i = 0; i < 6; i++) {
            inOutIndices.push_back(baseIndex + indices[i]);
        }
    }
};

class IFontRenderComponent : public IBasicRenderComponent {
public:
    virtual std::shared_ptr<FontMetadata> GetFontMetadata() const = 0;

public:
    virtual ~IFontRenderComponent() = default;
};

#endif // !__FONT_H__
