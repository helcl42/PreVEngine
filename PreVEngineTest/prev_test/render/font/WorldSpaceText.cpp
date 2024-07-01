#include "WorldSpaceText.h"

namespace prev_test::render::font {
WorldSpaceText::WorldSpaceText(const std::wstring& text, float fontSize, const glm::vec4& color, const glm::vec3& position, const glm::quat& orientation, bool alwaysFaceCamera, float maxLineLength, bool centered, float width, float edge)
    : AbstractText(text, fontSize, color, maxLineLength, centered, width, edge)
    , m_position(position)
    , m_orientation(orientation)
    , m_alwaysFaceCamera(alwaysFaceCamera)
{
}

WorldSpaceText::WorldSpaceText(const std::string& text, float fontSize, const glm::vec4& color, const glm::vec3& position, const glm::quat& orientation, bool alwaysFaceCamera, float maxLineLength, bool centered, float width, float edge)
    : AbstractText(text, fontSize, color, maxLineLength, centered, width, edge)
    , m_position(position)
    , m_orientation(orientation)
    , m_alwaysFaceCamera(alwaysFaceCamera)
{
}

const glm::vec3& WorldSpaceText::GetPosition() const
{
    return m_position;
}

const glm::quat& WorldSpaceText::GetOrientation() const
{
    return m_orientation;
}

bool WorldSpaceText::IsAwaysFacingCamera() const
{
    return m_alwaysFaceCamera;
}
} // namespace prev_test::render::font
