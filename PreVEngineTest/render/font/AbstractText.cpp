#include "AbstractText.h"

namespace prev_test::render::font {
AbstractText::AbstractText(const std::wstring& text, float fontSize, const glm::vec4& color, float maxLineLength, bool centered, float width, float edge)
    : m_textString(text)
    , m_fontSize(fontSize)
    , m_color(color)
    , m_lineMaxSize(maxLineLength)
    , m_centerText(centered)
    , m_width(width)
    , m_edge(edge)
{
}

AbstractText::AbstractText(const std::string& text, float fontSize, const glm::vec4& color, float maxLineLength, bool centered, float width, float edge)
    : m_fontSize(fontSize)
    , m_color(color)
    , m_lineMaxSize(maxLineLength)
    , m_centerText(centered)
    , m_width(width)
    , m_edge(edge)
{
    m_textString.assign(text.begin(), text.end());
}

void AbstractText::SetColor(const glm::vec4& color)
{
    m_color = color;
}

const glm::vec4& AbstractText::GetColor() const
{
    return m_color;
}

int AbstractText::GetNumberOfLines() const
{
    return m_numberOfLines;
}

float AbstractText::GetFontSize() const
{
    return m_fontSize;
}

void AbstractText::SetNumberOfLines(unsigned int number)
{
    m_numberOfLines = number;
}

bool AbstractText::IsCentered() const
{
    return m_centerText;
}

float AbstractText::GetMaxLineSize() const
{
    return m_lineMaxSize;
}

std::wstring AbstractText::GetTextString() const
{
    return m_textString;
}

float AbstractText::GetWidth() const
{
    return m_width;
}

float AbstractText::GetEdge() const
{
    return m_edge;
}

bool AbstractText::HasEffect() const
{
    return m_hasEffect;
}

float AbstractText::GetBorderWidth() const
{
    return m_borderWidth;
}

float AbstractText::GetBorderEdge() const
{
    return m_borderEdge;
}

glm::vec3 AbstractText::GetOutlineColor() const
{
    return m_outlineColor;
}

glm::vec2 AbstractText::GetOutlineOffset() const
{
    return m_outlineOffset;
}

void AbstractText::EnableEffect(float borderWidth, float borderEdge, const glm::vec3& outlineColor, const glm::vec2& outlineOffset)
{
    m_hasEffect = true;
    m_borderWidth = borderWidth;
    m_borderEdge = borderEdge;
    m_outlineColor = outlineColor;
    m_outlineOffset = outlineOffset;
}

void AbstractText::DisableEffect()
{
    m_hasEffect = false;
}
} // namespace prev_test::render::font
