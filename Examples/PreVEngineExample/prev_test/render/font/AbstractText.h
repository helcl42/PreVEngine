#ifndef __ABSTRACT_TEXT_H__
#define __ABSTRACT_TEXT_H__

#include <prev/common/Common.h>

namespace prev_test::render::font {
class AbstractText {
public:
    explicit AbstractText(const std::wstring& text, float fontSize, const glm::vec4& color, float maxLineLength, bool centered, float width, float edge);

    explicit AbstractText(const std::string& text, float fontSize, const glm::vec4& color, float maxLineLength, bool centered, float width, float edge);

    virtual ~AbstractText() = default;

public:
    void SetColor(const glm::vec4& color);

    const glm::vec4& GetColor() const;

    float GetFontSize() const;

    bool IsCentered() const;

    float GetMaxLineSize() const;

    std::wstring GetTextString() const;

    float GetWidth() const;

    float GetEdge() const;

    bool HasEffect() const;

    float GetBorderWidth() const;

    float GetBorderEdge() const;

    glm::vec3 GetOutlineColor() const;

    glm::vec2 GetOutlineOffset() const;

    void EnableEffect(float borderWidth, float borderEdge, const glm::vec3& outlineColor, const glm::vec2& outlineOffset);

    void DisableEffect();

protected:
    std::wstring m_textString;

    float m_fontSize;

    glm::vec4 m_color{ 0.0f, 0.0f, 0.0f, 1.0f };

    float m_lineMaxSize;

    bool m_centerText{ false };

    float m_width;

    float m_edge;

    bool m_hasEffect{ false };

    float m_borderWidth{ 0.0f };

    float m_borderEdge{ 0.0f };

    glm::vec3 m_outlineColor{ 1.0f };

    glm::vec2 m_outlineOffset{ 0.0f };
};
} // namespace prev_test::render::font

#endif // !__ABSTRACT_TEXT_H__
