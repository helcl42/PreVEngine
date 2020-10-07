#include "Character.h"

namespace prev_test::render::font {
Character::Character(const int id, const glm::vec2& textureCoord, const glm::vec2& texSize, const glm::vec2& offset, const glm::vec2& size, const float xAdvance)
    : m_id(id)
    , m_textureCoords(textureCoord)
    , m_maxTextureCoords(texSize + textureCoord)
    , m_offset(offset)
    , m_size(size)
    , m_xAdvance(xAdvance)
{
}

int Character::GetId() const
{
    return m_id;
}

const glm::vec2& Character::GetTextureCoords() const
{
    return m_textureCoords;
}

const glm::vec2& Character::GetMaxTextureCoords() const
{
    return m_maxTextureCoords;
}

const glm::vec2& Character::GetOffset() const
{
    return m_offset;
}

const glm::vec2& Character::GetSize() const
{
    return m_size;
}

float Character::GetXAdvance() const
{
    return m_xAdvance;
}
} // namespace prev_test::render::font
