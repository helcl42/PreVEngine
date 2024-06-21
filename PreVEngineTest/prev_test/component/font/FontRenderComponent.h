#ifndef __FONT_RENDER_COMPONENT_H__
#define __FONT_RENDER_COMPONENT_H__

#include "IFontRenderComponent.h"

namespace prev_test::component::font {
class FontRenderComponent : public IFontRenderComponent {
public:
    FontRenderComponent(const std::shared_ptr<prev_test::render::font::FontMetadata>& fontMetaData);

    ~FontRenderComponent() = default;

public:
    const std::map<uint32_t, RenderableText>& GetRenderableTexts() const override;

    std::shared_ptr<prev_test::render::font::FontMetadata> GetFontMetadata() const override;

    void AddText(const uint32_t key, const std::shared_ptr<prev_test::render::font::ScreenSpaceText>& text) override;

    void RemoveText(const uint32_t key) override;

    void Reset() override;

private:
    std::shared_ptr<prev_test::render::font::FontMetadata> m_fontMetaData;

    std::map<uint32_t, RenderableText> m_renderableTexts;
};
} // namespace prev_test::component::font

#endif // !__FONT_RENDER_COMPONENT_H__
