#include "WorldSpaceText.h"

namespace prev_test::render::font {
WorldSpaceText::WorldSpaceText(const std::wstring& text, float fontSize, const glm::vec4& color, const glm::vec3& position, const glm::vec3& orientation, bool ignoreOverallRotations, float maxLineLength, bool centered, float width, float edge)
    : AbstractText(text, fontSize, color, maxLineLength, centered, width, edge)
    , m_position(position)
    , m_orientation(orientation)
    , m_ignoreOverallRotations(ignoreOverallRotations)
{
}

WorldSpaceText::WorldSpaceText(const std::string& text, float fontSize, const glm::vec4& color, const glm::vec3& position, const glm::vec3& orientation, bool ignoreOverallRotations, float maxLineLength, bool centered, float width, float edge)
    : AbstractText(text, fontSize, color, maxLineLength, centered, width, edge)
    , m_position(position)
    , m_orientation(orientation)
    , m_ignoreOverallRotations(ignoreOverallRotations)
{
}

const glm::vec3& WorldSpaceText::GetPosition() const
{
    return m_position;
}

const glm::vec3& WorldSpaceText::GetOrientation() const
{
    return m_orientation;
}

bool WorldSpaceText::IsCameraRotationIgnored() const
{
    return m_ignoreOverallRotations;
}
} // namespace prev_test::render::font
