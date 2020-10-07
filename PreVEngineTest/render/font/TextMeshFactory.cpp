#include "TextMeshFactory.h"

#include "TextMesh.h"

namespace prev_test::render::font {
std::unique_ptr<prev_test::render::IMesh> TextMeshFactory::CreateTextMesh(const std::shared_ptr<AbstractText>& text, const std::shared_ptr<FontMetadata> fontMetaData) const
{
    std::vector<TextLine> lines;
    CreateStructure(text, fontMetaData, lines);
    return CreateQuadVertices(text, fontMetaData, lines);
}


void TextMeshFactory::CreateStructure(const std::shared_ptr<AbstractText>& text, const std::shared_ptr<FontMetadata> fontMetaData, std::vector<TextLine>& lines) const
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

void TextMeshFactory::CompleteStructure(const std::shared_ptr<AbstractText>& text, const std::shared_ptr<FontMetadata> fontMetaData, std::vector<TextLine>& lines, TextLine& currentLine, Word& currentWord) const
{
    const bool added = currentLine.AttemptToAddWord(currentWord);
    if (!added) {
        lines.push_back(currentLine);
        currentLine = TextLine{ text->GetMaxLineSize(), fontMetaData->GetFontSizeScaledSpaceWidth(text->GetFontSize()) };
        currentLine.AttemptToAddWord(currentWord);
    }
    lines.push_back(currentLine);
}

std::unique_ptr<prev_test::render::IMesh> TextMeshFactory::CreateQuadVertices(const std::shared_ptr<AbstractText>& text, const std::shared_ptr<FontMetadata> fontMetaData, const std::vector<TextLine>& lines) const
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

void TextMeshFactory::AddVerticesForCharacter(const float curserX, const float curserY, const Character& character, const float fontSize, std::vector<glm::vec2>& vertices) const
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

void TextMeshFactory::AddQuadData(const float x, const float y, const float maxX, const float maxY, std::vector<glm::vec2>& vertices) const
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

void TextMeshFactory::AddIndices(const size_t verticesCount, std::vector<uint32_t>& inOutIndices) const
{
    const uint32_t indices[] = { 0, 3, 2, 2, 1, 0 };
    const uint32_t baseIndex = static_cast<uint32_t>(verticesCount);
    for (uint32_t i = 0; i < 6; i++) {
        inOutIndices.push_back(baseIndex + indices[i]);
    }
}
}