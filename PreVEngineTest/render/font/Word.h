#ifndef __WORD_H__
#define __WORD_H__

#include "Character.h"

#include <vector>

namespace prev_test::render::font {
class Word {
public:
    explicit Word(const float fontSize);

    virtual ~Word() = default;

public:
    void AddCharacter(const Character& character);

    const std::vector<Character>& GetCharacters() const;

    float GetWordWidth() const;

    float GetFontSize() const;

private:
    std::vector<Character> m_characters;

    float m_fontSize;

    float m_width;
};
} // namespace prev_test::render::font

#endif // !__WORD_H__
