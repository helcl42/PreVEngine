#include "TextMeshFactory.h"

#include "../mesh/MeshFactory.h"

namespace prev_test::render::font {
std::unique_ptr<prev_test::render::IMesh> TextMeshFactory::CreateTextMesh(const AbstractText& text, const FontMetadata& fontMetaData) const
{
    const auto lines{ CreateStructure(text, fontMetaData) };
    return CreateQuadVertices(text, fontMetaData, lines);
}

std::vector<TextLine> TextMeshFactory::CreateStructure(const AbstractText& text, const FontMetadata& fontMetaData) const
{
    std::vector<TextLine> lines;

    TextLine currentLine{ text.GetMaxLineSize(), fontMetaData.GetSpaceWidth(text.GetFontSize()) };

    Word currentWord{ text.GetFontSize() };
    for (const auto& c : text.GetTextString()) {
        const int charCode{ static_cast<int>(c) };
        if (charCode == FontMetadata::SPACE_CHARACTER) {
            const bool added = currentLine.AttemptToAddWord(currentWord);
            if (!added) {
                lines.push_back(currentLine);
                currentLine = TextLine{ text.GetMaxLineSize(), fontMetaData.GetSpaceWidth(text.GetFontSize()) };
                currentLine.AttemptToAddWord(currentWord);
            }
            currentWord = Word{ text.GetFontSize() };
            continue;
        } else if (charCode == FontMetadata::NEW_LINE_CHARACTER) {
            const bool added{ currentLine.AttemptToAddWord(currentWord) };
            lines.push_back(currentLine);
            currentLine = TextLine{ text.GetMaxLineSize(), fontMetaData.GetSpaceWidth(text.GetFontSize()) };
            if (!added) {
                currentLine.AttemptToAddWord(currentWord);
            }
            currentWord = Word{ text.GetFontSize() };
            continue;
        }

        Character character{};
        if (!fontMetaData.GetCharacter(charCode, character)) {
            fontMetaData.GetCharacter(FontMetadata::UNKNOWN_CHARACTER, character);
        }
        currentWord.AddCharacter(character);
    }

    const bool added{ currentLine.AttemptToAddWord(currentWord) };
    if (!added) {
        lines.push_back(currentLine);
        currentLine = TextLine{ text.GetMaxLineSize(), fontMetaData.GetSpaceWidth(text.GetFontSize()) };
        currentLine.AttemptToAddWord(currentWord);
    }
    lines.push_back(currentLine);

    return lines;
}

std::unique_ptr<prev_test::render::IMesh> TextMeshFactory::CreateQuadVertices(const AbstractText& text, const FontMetadata& fontMetaData, const std::vector<TextLine>& lines) const
{
    std::vector<glm::vec2> vertices;
    std::vector<glm::vec2> textureCoords;
    std::vector<uint32_t> indices;

    glm::vec2 curser{ 0.0f, 0.0f };
    for (const auto& line : lines) {
        if (text.IsCentered()) {
            curser.x = (line.GetMaxLength() - line.GetCurrentLength()) / 2.0f;
        }

        for (const auto& word : line.GetWords()) {
            for (const auto& character : word.GetCharacters()) {
                AddIndices(vertices.size(), indices);
                AddVerticesForCharacter(curser, character, text.GetFontSize(), vertices);
                AddQuadData(character.GetTextureCoords(), character.GetMaxTextureCoords(), textureCoords);
                curser.x += character.GetXAdvance() * text.GetFontSize();
            }
            curser.x += fontMetaData.GetSpaceWidth(text.GetFontSize());
        }
        curser.x = 0;
        curser.y += fontMetaData.GetLineHeight(text.GetFontSize());
    }

    return prev_test::render::mesh::MeshFactory{}.CreateFromData(vertices, textureCoords, indices);
}

void TextMeshFactory::AddVerticesForCharacter(const glm::vec2& curser, const Character& character, const float fontSize, std::vector<glm::vec2>& inOutVertices) const
{
    const float minX{ curser.x + (character.GetOffset().x * fontSize) };
    const float minY{ curser.y + (character.GetOffset().y * fontSize) };
    const float maxX{ minX + (character.GetSize().x * fontSize) };
    const float maxY{ minY + (character.GetSize().y * fontSize) };

    const glm::vec2 topLeftPoint{ glm::vec2{ minX, minY } * 2.0f - 1.0f };
    const glm::vec2 bottomRightPoint{ glm::vec2{ maxX, maxY } * 2.0f - 1.0f };

    AddQuadData(topLeftPoint, bottomRightPoint, inOutVertices);
}

void TextMeshFactory::AddQuadData(const glm::vec2& tl, const glm::vec2& br, std::vector<glm::vec2>& inOutVertices) const
{
    const glm::vec2 quadVertices[] = {
        { tl.x, tl.y },
        { br.x, tl.y },
        { br.x, br.y },
        { tl.x, br.y }
    };

    for (uint32_t i = 0; i < 4; i++) {
        inOutVertices.push_back(quadVertices[i]);
    }
}

void TextMeshFactory::AddIndices(const size_t verticesCount, std::vector<uint32_t>& inOutIndices) const
{
    const uint32_t indices[] = { 0, 2, 1, 2, 0, 3 };
    const uint32_t baseIndex = static_cast<uint32_t>(verticesCount);
    for (uint32_t i = 0; i < 6; i++) {
        inOutIndices.push_back(baseIndex + indices[i]);
    }
}
} // namespace prev_test::render::font