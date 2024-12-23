#ifndef __WORLD_SPACE_TEXT_H__
#define __WORLD_SPACE_TEXT_H__

#include "AbstractText.h"

namespace prev_test::render::font {
class WorldSpaceText : public AbstractText {
public:
    explicit WorldSpaceText(const std::wstring& text, float fontSize, const glm::vec4& color, const glm::vec3& position, const glm::quat& orientation, bool alwaysFaceCamera, float maxLineLength, bool centered, float width, float edge);

    explicit WorldSpaceText(const std::string& text, float fontSize, const glm::vec4& color, const glm::vec3& position, const glm::quat& orientation, bool alwaysFaceCamera, float maxLineLength, bool centered, float width, float edge);

    virtual ~WorldSpaceText() = default;

public:
    const glm::vec3& GetPosition() const;

    const glm::quat& GetOrientation() const;

    bool IsAlwaysFacingCamera() const;

private:
    glm::vec3 m_position;

    glm::quat m_orientation;

    bool m_alwaysFaceCamera;
};
} // namespace prev_test::render::font

#endif // !__WORLD_SPACE_TEXT_H__
