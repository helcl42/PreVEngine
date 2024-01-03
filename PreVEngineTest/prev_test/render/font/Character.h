#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include <prev/common/Common.h>

namespace prev_test::render::font {
class Character {
public:
    explicit Character() = default;

    explicit Character(const int id, const glm::vec2& textureCoord, const glm::vec2& texSize, const glm::vec2& offset, const glm::vec2& size, const float xAdvance);

    ~Character() = default;

public:
    int GetId() const;

    const glm::vec2& GetTextureCoords() const;

    const glm::vec2& GetMaxTextureCoords() const;

    const glm::vec2& GetOffset() const;

    const glm::vec2& GetSize() const;

    float GetXAdvance() const;

private:
    int m_id;

    glm::vec2 m_textureCoords;

    glm::vec2 m_maxTextureCoords;

    glm::vec2 m_offset;

    glm::vec2 m_size;

    float m_xAdvance;
};
} // namespace prev_test::render::font

#endif // !__CHARACTER_H__
