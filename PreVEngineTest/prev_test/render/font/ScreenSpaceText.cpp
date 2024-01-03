#include "ScreenSpaceText.h"

namespace prev_test::render::font {
ScreenSpaceText::ScreenSpaceText(const std::wstring& text, float fontSize, const glm::vec4& color, const glm::vec2& position, float maxLineLength, bool centered, float width, float edge)
    : AbstractText(text, fontSize, color, maxLineLength, centered, width, edge)
    , m_position(position)
{
}

ScreenSpaceText::ScreenSpaceText(const std::string& text, float fontSize, const glm::vec4& color, const glm::vec2& position, float maxLineLength, bool centered, float width, float edge)
    : AbstractText(text, fontSize, color, maxLineLength, centered, width, edge)
    , m_position(position)
{
}

const glm::vec2& ScreenSpaceText::GetPosition() const
{
    return m_position;
}
} // namespace prev_test::render::font
