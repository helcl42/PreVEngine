#ifndef __DEFAULT_FONT_RENDER_COMPONENT_H__
#define __DEFAULT_FONT_RENDER_COMPONENT_H__

#include "IFontRenderComponent.h"

namespace prev_test::component::font {
class DefaultFontRenderComponent : public IFontRenderComponent {
private:
    std::shared_ptr<prev_test::render::font::FontMetadata> m_fontMetaData;

    std::vector<RenderableText> m_renderableTexts;

public:
    DefaultFontRenderComponent(const std::shared_ptr<prev_test::render::font::FontMetadata>& fontMetaData);

    ~DefaultFontRenderComponent() = default;

public:
    const std::vector<RenderableText>& GetRenderableTexts() const;

    std::shared_ptr<prev_test::render::font::FontMetadata> GetFontMetadata() const;

    void AddText(const std::shared_ptr<prev_test::render::font::ScreenSpaceText>& text);

    void Reset();
};
} // namespace prev_test::component::font

#endif // !__DEFAULT_FONT_RENDER_COMPONENT_H__
