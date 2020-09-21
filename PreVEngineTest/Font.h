#ifndef __FONT_H__
#define __FONT_H__

#include "General.h"

#include "render/VertexLayout.h"
#include "render/VertexDataBuffer.h"

#include <prev/common/Common.h>
#include <prev/render/image/ImageFactory.h>
#include <prev/core/memory/image/ImageBuffer.h>
#include <prev/util/Utils.h>

#include <fstream>
#include <iostream>
#include <map>

class Character;
class Word;
class TextLine;
class FontMetadata;
class FontMetadataFactory;
class TextMesh;
class TextMeshFactory;

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

    explicit Character(const int id, const glm::vec2& textureCoord, const glm::vec2& texSize, const glm::vec2& offset, const glm::vec2& size, const float xAdvance)
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

    const glm::vec2& GetTextureCoords() const
    {
        return m_textureCoords;
    }

    const glm::vec2& GetMaxTextureCoords() const
    {
        return m_maxTextureCoords;
    }

    const glm::vec2& GetOffset() const
    {
        return m_offset;
    }

    const glm::vec2& GetSize() const
    {
        return m_size;
    }

    float GetXAdvance() const
    {
        return m_xAdvance;
    }
};

class FontMetadata {
private:
    friend class FontMetadataFactory;

private:
    float m_spaceWidth;

    std::map<int, Character> m_characterMetaData;

    std::shared_ptr<prev::render::image::Image> m_image;

    std::shared_ptr<prev::core::memory::image::ImageBuffer> m_imageBuffer;

public:
    inline static const float LINE_HEIGHT{ 0.03f };

    inline static const int NULL_CODE{ 0 };

    inline static const int SPACE_CODE{ ' ' };

    inline static const int NEW_LINE_CODE{ '\n' };

    inline static const int FALLBACK_CODE{ '?' };

public:
    float GetSpaceWidth() const
    {
        return m_spaceWidth;
    }

    float GetFontSizeScaledSpaceWidth(const float fontSize)
    {
        return m_spaceWidth * fontSize;
    }

    std::shared_ptr<prev::render::image::Image> GetImage() const
    {
        return m_image;
    }

    std::shared_ptr<prev::core::memory::image::IImageBuffer> GetImageBuffer() const
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
        auto numberStrings = prev::util::StringUtils::Split(m_currentLine.at(variable), ',');
        for (size_t i = 0; i < numberStrings.size(); i++) {
            actualValues.push_back(std::stoi(numberStrings[i]));
        }
        return actualValues;
    }

private:
    bool ProcessNextLine(std::ifstream& inOutFileStream, std::map<std::string, std::string>& outTokens) const
    {
        outTokens.clear();

        std::string line;
        std::getline(inOutFileStream, line);

        if (line.empty()) {
            return false;
        }

        auto items = prev::util::StringUtils::Split(line, ' ');
        for (auto& part : items) {
            auto valuePairs = prev::util::StringUtils::Split(part, '=');
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

private:
    std::vector<std::map<std::string, std::string> > m_allLinesKeyValues;

    std::map<std::string, std::string> m_currentLine;

    uint32_t m_lineNumber = 0;
};

class FontMetadataFactory {
private:
    inline static const int PADDING_TOP_INDEX{ 0 };
    inline static const int PADDING_LEFT_INDEX{ 1 };
    inline static const int PADDING_BOTTOM_INDEX{ 2 };
    inline static const int PADDING_RIGHT_INDEX{ 3 };

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

    void CreateImage(const std::string& textureFilePath, std::shared_ptr<prev::render::image::Image>& image, std::shared_ptr<prev::core::memory::image::ImageBuffer>& imageBuffer) const
    {
        prev::render::image::ImageFactory imageFactory;
        image = imageFactory.CreateImage(textureFilePath);
        imageBuffer = std::make_unique<prev::core::memory::image::ImageBuffer>(*prev::scene::AllocatorProvider::Instance().GetAllocator());
        imageBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ VkExtent2D{ image->GetWidth(), image->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, true, VK_IMAGE_VIEW_TYPE_2D, 1, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, (uint8_t*)image->GetBuffer() });
    }

    void ExtractCharactersData(MetaDataFile& metaDataFile, FontMetaDataState& state, std::map<int, Character>& characters) const
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

    Character CreateSingleCharacter(MetaDataFile& metaDataFile, const FontMetaDataState& state, const int charCode) const
    {
        const float xTex = (static_cast<float>(metaDataFile.GetValueAsInt("x")) + (state.padding[PADDING_LEFT_INDEX] - state.desiredPadding)) / state.imageWidth;
        const float yTex = (static_cast<float>(metaDataFile.GetValueAsInt("y")) + (state.padding[PADDING_TOP_INDEX] - state.desiredPadding)) / state.imageWidth;
        const int width = metaDataFile.GetValueAsInt("width") - (state.paddingWidth - (2 * state.desiredPadding));
        const int height = metaDataFile.GetValueAsInt("height") - ((state.paddingHeight) - (2 * state.desiredPadding));
        const float quadWidth = width * state.horizontalPerPixelSize;
        const float quadHeight = height * state.verticalPerPixelSize;
        const float xTexSize = static_cast<float>(width) / state.imageWidth;
        const float yTexSize = static_cast<float>(height) / state.imageWidth;
        const float xOff = (metaDataFile.GetValueAsInt("xoffset") + state.padding[PADDING_LEFT_INDEX] - state.desiredPadding) * state.horizontalPerPixelSize;
        const float yOff = (metaDataFile.GetValueAsInt("yoffset") + (state.padding[PADDING_TOP_INDEX] - state.desiredPadding)) * state.verticalPerPixelSize;
        const float xAdvance = (metaDataFile.GetValueAsInt("xadvance") - state.paddingWidth) * state.horizontalPerPixelSize;
        return Character(charCode, glm::vec2(xTex, yTex), glm::vec2(xTexSize, yTexSize), glm::vec2(xOff, yOff), glm::vec2(quadWidth, quadHeight), xAdvance);
    }

public:
    std::unique_ptr<FontMetadata> CreateFontMetadata(const std::string& metadataFilePath, const std::string& textureFilePath, const float aspectRatio = 16.0f / 9.0f, const int desiredPadding = 0) const
    {
        MetaDataFile metaDataFile{ metadataFilePath };

        FontMetaDataState state{};
        state.desiredPadding = desiredPadding;
        state.aspectRatio = aspectRatio;
        ExtractPaddingData(metaDataFile, state);
        ExtractMeasureInfo(metaDataFile, state);

        std::shared_ptr<prev::render::image::Image> image;
        std::shared_ptr<prev::core::memory::image::ImageBuffer> imagwBuffer;
        CreateImage(textureFilePath, image, imagwBuffer);

        std::map<int, Character> characterMetaData{};
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

    float m_fontSize;

    float m_width;

public:
    explicit Word(const float fontSize)
        : m_fontSize(fontSize)
        , m_width(0)
    {
    }

    virtual ~Word() = default;

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

    float m_spaceWidth;

    float m_currentLength;

    std::vector<Word> m_words;

public:
    explicit TextLine(const float maxLength, const float spaceWidth)
        : m_maxLength(maxLength)
        , m_spaceWidth(spaceWidth)
        , m_currentLength(0)
    {
    }

    ~TextLine() = default;

public:
    bool AttemptToAddWord(const Word& word)
    {
        const float additionalLength = word.GetWordWidth() + (m_words.empty() ? 0 : m_spaceWidth);
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
    explicit BaseFancyText(const std::wstring& text, float fontSize, const glm::vec4& color, float maxLineLength, bool centered, float width, float edge)
        : m_textString(text)
        , m_fontSize(fontSize)
        , m_color(color)
        , m_lineMaxSize(maxLineLength)
        , m_centerText(centered)
        , m_width(width)
        , m_edge(edge)
        , m_hasEffect(false)
    {
    }

    explicit BaseFancyText(const std::string& text, float fontSize, const glm::vec4& color, float maxLineLength, bool centered, float width, float edge)
        : m_fontSize(fontSize)
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
    explicit FancyText(const std::wstring& text, float fontSize, const glm::vec4& color, const glm::vec2& position, float maxLineLength, bool centered, float width, float edge)
        : BaseFancyText(text, fontSize, color, maxLineLength, centered, width, edge)
        , m_position(position)
    {
    }

    explicit FancyText(const std::string& text, float fontSize, const glm::vec4& color, const glm::vec2& position, float maxLineLength, bool centered, float width, float edge)
        : BaseFancyText(text, fontSize, color, maxLineLength, centered, width, edge)
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
    explicit FancyTextInSpace(const std::wstring& text, float fontSize, const glm::vec4& color, const glm::vec3& position, const glm::vec3& orientation, bool ignoreOverallRotations, float maxLineLength, bool centered, float width, float edge)
        : BaseFancyText(text, fontSize, color, maxLineLength, centered, width, edge)
        , m_position(position)
        , m_orientation(orientation)
        , m_ignoreOverallRotations(ignoreOverallRotations)
    {
    }

    explicit FancyTextInSpace(const std::string& text, float fontSize, const glm::vec4& color, const glm::vec3& position, const glm::vec3& orientation, bool ignoreOverallRotations, float maxLineLength, bool centered, float width, float edge)
        : BaseFancyText(text, fontSize, color, maxLineLength, centered, width, edge)
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
public:
    TextMesh(const std::vector<glm::vec2>& vertices, const std::vector<glm::vec2>& textureCoords, const std::vector<uint32_t>& indices)
        : m_vertexLayout({ prev_test::render::VertexLayoutComponent::VEC2, prev_test::render::VertexLayoutComponent::VEC2 })
        , m_verticesCount(static_cast<uint32_t>(vertices.size()))
        , m_indices(indices)
    {
        for (size_t i = 0; i < vertices.size(); i++) {
            m_vertexDataBuffer.Add(vertices[i]);
            m_vertexDataBuffer.Add(textureCoords[i]);
            m_vertices.push_back(glm::vec3(vertices[i].x, vertices[i].y, 0.0f));
        }

        m_meshParts.push_back(MeshPart(static_cast<uint32_t>(m_indices.size())));
    }

    ~TextMesh() = default;

public:
    const prev_test::render::VertexLayout& GetVertexLayout() const override
    {
        return m_vertexLayout;
    }

    const void* GetVertexData() const override
    {
        return m_vertexDataBuffer.GetData();
    }

    const std::vector<glm::vec3>& GetVertices() const override
    {
        return m_vertices;
    }

    uint32_t GerVerticesCount() const override
    {
        return m_verticesCount;
    }

    const std::vector<uint32_t>& GetIndices() const override
    {
        return m_indices;
    }
    
    const std::vector<MeshPart>& GetMeshParts() const override
    {
        return m_meshParts;
    }

private:
    prev_test::render::VertexLayout m_vertexLayout;

    std::vector<glm::vec3> m_vertices;

    uint32_t m_verticesCount;

    std::vector<uint32_t> m_indices;

    prev_test::render::VertexDataBuffer m_vertexDataBuffer;

    std::vector<MeshPart> m_meshParts;
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
        TextLine currentLine{ text->GetMaxLineSize(), fontMetaData->GetFontSizeScaledSpaceWidth(text->GetFontSize()) };
        Word currentWord{ text->GetFontSize() };
        for (auto c : text->GetTextString()) {
            const int charCode = static_cast<int>(c);
            if (charCode == FontMetadata::SPACE_CODE) {
                const bool added = currentLine.AttemptToAddWord(currentWord);
                if (!added) {
                    lines.push_back(currentLine);
                    currentLine = TextLine{ text->GetMaxLineSize(), fontMetaData->GetFontSizeScaledSpaceWidth(text->GetFontSize()) };
                    currentLine.AttemptToAddWord(currentWord);
                }
                currentWord = Word{ text->GetFontSize() };
                continue;
            } else if (charCode == FontMetadata::NEW_LINE_CODE) {
                const bool added = currentLine.AttemptToAddWord(currentWord);
                lines.push_back(currentLine);
                currentLine = TextLine{ text->GetMaxLineSize(), fontMetaData->GetFontSizeScaledSpaceWidth(text->GetFontSize()) };
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
        const bool added = currentLine.AttemptToAddWord(currentWord);
        if (!added) {
            lines.push_back(currentLine);
            currentLine = TextLine{ text->GetMaxLineSize(), fontMetaData->GetFontSizeScaledSpaceWidth(text->GetFontSize()) };
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
                    AddIndices(vertices.size(), indices);
                    AddVerticesForCharacter(curserX, curserY, letter, text->GetFontSize(), vertices);
                    AddQuadData(letter.GetTextureCoords().x, letter.GetTextureCoords().y, letter.GetMaxTextureCoords().x, letter.GetMaxTextureCoords().y, textureCoords);
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

        AddQuadData(properX, 1.0f - properY, properMaxX, 1.0f - properMaxY, vertices);
    }

    void AddQuadData(const float x, const float y, const float maxX, const float maxY, std::vector<glm::vec2>& vertices) const
    {
        const glm::vec2 planeVertices[] = {
            { x, y },
            { maxX, y },
            { maxX, maxY },
            { x, maxY }
        };

        for (uint32_t i = 0; i < 4; i++) {
            vertices.push_back(planeVertices[i]);
        }
    }

    void AddIndices(const size_t verticesCount, std::vector<uint32_t>& inOutIndices) const
    {
        const uint32_t indices[] = { 0, 3, 2, 2, 1, 0 };
        const uint32_t baseIndex = static_cast<uint32_t>(verticesCount);
        for (uint32_t i = 0; i < 6; i++) {
            inOutIndices.push_back(baseIndex + indices[i]);
        }
    }
};

struct RenderableText {
    std::shared_ptr<FancyText> text;

    std::shared_ptr<IModel> model;
};

class IFontRenderComponent {
public:
    virtual std::shared_ptr<FontMetadata> GetFontMetadata() const = 0;

    virtual const std::vector<RenderableText>& GetRenderableTexts() const = 0;

    virtual void AddText(const std::shared_ptr<FancyText>& text) = 0;

    virtual void Reset() = 0;

public:
    virtual ~IFontRenderComponent() = default;
};

class DefaultFontRenderComponent : public IFontRenderComponent {
private:
    std::shared_ptr<FontMetadata> m_fontMetaData;

    std::vector<RenderableText> m_renderableTexts;

public:
    DefaultFontRenderComponent(const std::shared_ptr<FontMetadata>& fontMetaData)
        : m_fontMetaData(fontMetaData)
    {
    }

    ~DefaultFontRenderComponent() = default;

public:
    const std::vector<RenderableText>& GetRenderableTexts() const override
    {
        return m_renderableTexts;
    }

    std::shared_ptr<FontMetadata> GetFontMetadata() const override
    {
        return m_fontMetaData;
    }

    void AddText(const std::shared_ptr<FancyText>& text) override
    {
        TextMeshFactory meshFactory;
        auto mesh = meshFactory.CreateTextMesh(text, m_fontMetaData);

        auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();
        auto vertexBuffer = std::make_shared<prev::core::memory::buffer::VertexBuffer>(*allocator);
        vertexBuffer->Data(mesh->GetVertexData(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());

        auto indexBuffer = std::make_shared<prev::core::memory::buffer::IndexBuffer>(*allocator);
        indexBuffer->Data(mesh->GetIndices().data(), (uint32_t)mesh->GetIndices().size());

        auto model = std::make_shared<Model>(std::move(mesh), vertexBuffer, indexBuffer);

        m_renderableTexts.push_back(RenderableText{ text, model });
    }

    void Reset() override
    {
        m_renderableTexts.clear();
    }
};

class FontRenderComponentsFactory {
public:
    std::unique_ptr<IFontRenderComponent> Create(const std::string& fontPath, const std::string& fontTexture, const float aspectRatio) const
    {
        FontMetadataFactory fontFactory{};
        auto fontMetaData = fontFactory.CreateFontMetadata(fontPath, fontTexture, aspectRatio);

        return std::make_unique<DefaultFontRenderComponent>(std::move(fontMetaData));
    }
};

#endif // !__FONT_H__
