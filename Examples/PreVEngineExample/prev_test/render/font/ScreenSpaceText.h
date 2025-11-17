#ifndef __SCREEN_SPACE_TEXT_H__
#define __SCREEN_SPACE_TEXT_H__

#include "AbstractText.h"

namespace prev_test::render::font {
class ScreenSpaceText : public AbstractText {
public:
    explicit ScreenSpaceText(const std::wstring& text, float fontSize, const glm::vec4& color, const glm::vec2& position, float maxLineLength, bool centered, float width, float edge);

    explicit ScreenSpaceText(const std::string& text, float fontSize, const glm::vec4& color, const glm::vec2& position, float maxLineLength, bool centered, float width, float edge);

    virtual ~ScreenSpaceText() = default;

public:
    const glm::vec2& GetPosition() const;

private:
    glm::vec2 m_position;
};
} // namespace prev_test::render::font

#endif // !__SCREEN_SPACE_TEXT_H__
