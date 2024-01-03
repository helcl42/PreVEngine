#ifndef __TEXT_LINE_H__
#define __TEXT_LINE_H__

#include "Word.h"

#include <vector>

namespace prev_test::render::font {
class TextLine {
public:
    explicit TextLine(const float maxLength, const float spaceWidth);

    ~TextLine() = default;

public:
    bool AttemptToAddWord(const Word& word);

    float GetMaxLength() const;

    float GetCurrentLength() const;

    const std::vector<Word>& GetWords() const;

private:
    float m_maxLength;

    float m_spaceWidth;

    float m_currentLength;

    std::vector<Word> m_words;
};
} // namespace prev_test::render::font

#endif // !__TEXT_LINE_H__
