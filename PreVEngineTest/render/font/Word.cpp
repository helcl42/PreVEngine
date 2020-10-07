#include "Word.h"

namespace prev_test::render::font {
Word::Word(const float fontSize)
    : m_fontSize(fontSize)
    , m_width(0)
{
}

void Word::AddCharacter(const Character& character)
{
    m_characters.push_back(character);
    m_width += character.GetXAdvance() * m_fontSize;
}

const std::vector<Character>& Word::GetCharacters() const
{
    return m_characters;
}

float Word::GetWordWidth() const
{
    return m_width;
}

float Word::GetFontSize() const
{
    return m_fontSize;
}

} // namespace prev_test::render::font
