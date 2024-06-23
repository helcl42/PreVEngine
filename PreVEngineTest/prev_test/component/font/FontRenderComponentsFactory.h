#ifndef __FONT_RENDER_COMPONENT_FACTORY_H__
#define __FONT_RENDER_COMPONENT_FACTORY_H__

#include "FontRenderComponent.h"

namespace prev_test::component::font {
class FontRenderComponentsFactory {
public:
    template <typename TextType>
    std::unique_ptr<IFontRenderComponent<TextType>> Create(const std::string& fontPath, const std::string& fontTexture, const float aspectRatio, const int padding) const
    {
        auto fontMetaData = CreateFontMetadata(fontPath, fontTexture, aspectRatio, padding);
        return std::make_unique<FontRenderComponent<TextType>>(std::move(fontMetaData));
    }

private:
    std::unique_ptr<prev_test::render::font::FontMetadata> CreateFontMetadata(const std::string& fontPath, const std::string& fontTexture, const float aspectRatio, const int padding) const;
};
} // namespace prev_test::component::font

#endif // !__FONT_RENDER_COMPONENT_FACTORY_H__
