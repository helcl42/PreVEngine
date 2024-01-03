#include "TextLine.h"

namespace prev_test::render::font {
TextLine::TextLine(const float maxLength, const float spaceWidth)
    : m_maxLength(maxLength)
    , m_spaceWidth(spaceWidth)
    , m_currentLength(0)
{
}

bool TextLine::AttemptToAddWord(const Word& word)
{
    const float additionalLength = word.GetWordWidth() + (m_words.empty() ? 0 : m_spaceWidth);
    if (m_currentLength + additionalLength <= m_maxLength) {
        m_words.push_back(word);
        m_currentLength += additionalLength;
        return true;
    }
    return false;
}

float TextLine::GetMaxLength() const
{
    return m_maxLength;
}

float TextLine::GetCurrentLength() const
{
    return m_currentLength;
}

const std::vector<Word>& TextLine::GetWords() const
{
    return m_words;
}
} // namespace prev_test::render::font
