#ifndef __FONT_RENDER_COMPONENT_FACTORY_H__
#define __FONT_RENDER_COMPONENT_FACTORY_H__

#include "IFontRenderComponent.h"

namespace prev_test::component::font {
class FontRenderComponentsFactory {
public:
    std::unique_ptr<IFontRenderComponent> Create(const std::string& fontPath, const std::string& fontTexture, const float aspectRatio, const int padding) const;
};
} // namespace prev_test::component::font

#endif // !__FONT_RENDER_COMPONENT_FACTORY_H__
