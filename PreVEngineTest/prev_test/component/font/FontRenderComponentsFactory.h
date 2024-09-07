#ifndef __FONT_RENDER_COMPONENT_FACTORY_H__
#define __FONT_RENDER_COMPONENT_FACTORY_H__

#include "FontRenderComponent.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>

namespace prev_test::component::font {
class FontRenderComponentsFactory final {
public:
    FontRenderComponentsFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator);

    ~FontRenderComponentsFactory() = default;

public:
    template <typename TextType>
    std::unique_ptr<IFontRenderComponent<TextType>> Create(const std::string& fontPath, const std::string& fontTexture, const float aspectRatio, const int padding) const
    {
        auto fontMetaData{ CreateFontMetadata(fontPath, fontTexture, aspectRatio, padding) };
        return std::make_unique<FontRenderComponent<TextType>>(m_allocator, std::move(fontMetaData));
    }

private:
    std::unique_ptr<prev_test::render::font::FontMetadata> CreateFontMetadata(const std::string& fontPath, const std::string& fontTexture, const float aspectRatio, const int padding) const;

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;
};
} // namespace prev_test::component::font

#endif // !__FONT_RENDER_COMPONENT_FACTORY_H__
