#ifndef __IFONT_RENDER_COMPONENT_H__
#define __IFONT_RENDER_COMPONENT_H__

#include "../../render/IModel.h"
#include "../../render/font/FontMetadata.h"
#include "../../render/font/ScreenSpaceText.h"

namespace prev_test::component::font {
struct RenderableText {
    std::shared_ptr<prev_test::render::font::ScreenSpaceText> text;

    std::shared_ptr<prev_test::render::IModel> model;
};

class IFontRenderComponent {
public:
    virtual std::shared_ptr<prev_test::render::font::FontMetadata> GetFontMetadata() const = 0;

    virtual const std::vector<RenderableText>& GetRenderableTexts() const = 0;

    virtual void AddText(const std::shared_ptr<prev_test::render::font::ScreenSpaceText>& text) = 0;

    virtual void Reset() = 0;

public:
    virtual ~IFontRenderComponent() = default;
};
} // namespace prev_test::component::font

#endif // !__IFONT_RENDER_COMPONENT_H__
