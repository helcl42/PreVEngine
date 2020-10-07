#ifndef __TEXT_MESH_FACTORY_H__
#define __TEXT_MESH_FACTORY_H__

#include "AbstractText.h"
#include "FontMetadata.h"
#include "TextLine.h"

#include "../IMesh.h"

#include <memory>

namespace prev_test::render::font {
class TextMeshFactory {
public:
    std::unique_ptr<prev_test::render::IMesh> CreateTextMesh(const std::shared_ptr<AbstractText>& text, const std::shared_ptr<FontMetadata> fontMetaData) const;

private:
    void CreateStructure(const std::shared_ptr<AbstractText>& text, const std::shared_ptr<FontMetadata> fontMetaData, std::vector<TextLine>& lines) const;

    void CompleteStructure(const std::shared_ptr<AbstractText>& text, const std::shared_ptr<FontMetadata> fontMetaData, std::vector<TextLine>& lines, TextLine& currentLine, Word& currentWord) const;

    std::unique_ptr<prev_test::render::IMesh> CreateQuadVertices(const std::shared_ptr<AbstractText>& text, const std::shared_ptr<FontMetadata> fontMetaData, const std::vector<TextLine>& lines) const;

    void AddVerticesForCharacter(const float curserX, const float curserY, const Character& character, const float fontSize, std::vector<glm::vec2>& vertices) const;

    void AddQuadData(const float x, const float y, const float maxX, const float maxY, std::vector<glm::vec2>& vertices) const;

    void AddIndices(const size_t verticesCount, std::vector<uint32_t>& inOutIndices) const;
};
} // namespace prev_test::render::font

#endif // !__TEXT_MESH_FACTORY_H__
