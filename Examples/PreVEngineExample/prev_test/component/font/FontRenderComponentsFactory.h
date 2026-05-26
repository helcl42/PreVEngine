#ifndef __FONT_RENDER_COMPONENT_FACTORY_H__
#define __FONT_RENDER_COMPONENT_FACTORY_H__

#include "FontRenderComponent.h"

#include <prev/core/device/Device.h>

namespace prev_test::component::font {
class FontRenderComponentsFactory final {
public:
    FontRenderComponentsFactory(prev::core::device::Device& device);

    ~FontRenderComponentsFactory() = default;

public:
    template <typename TextType>
    std::unique_ptr<IFontRenderComponent<TextType>> Create(const std::string& fontPath, const std::string& fontTexture, const float aspectRatio, const float lineHeight, const int padding) const
    {
        auto fontMetaData{ CreateFontMetadata(fontPath, fontTexture, aspectRatio, lineHeight, padding) };
        return std::make_unique<FontRenderComponent<TextType>>(m_device, std::move(fontMetaData));
    }

private:
    std::unique_ptr<prev_test::render::font::FontMetadata> CreateFontMetadata(const std::string& fontPath, const std::string& fontTexture, const float aspectRatio, const float lineHeight, const int padding) const;

private:
    prev::core::device::Device& m_device;
};
} // namespace prev_test::component::font

#endif // !__FONT_RENDER_COMPONENT_FACTORY_H__
