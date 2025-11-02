#ifndef __FONT_METADATA_H__
#define __FONT_METADATA_H__

#include "Character.h"

#include <prev/render/buffer/ImageBuffer.h>

#include <map>
#include <memory>

namespace prev_test::render::font {
class FontMetadata {
public:
    inline static const float LINE_HEIGHT{ 0.03f };

    inline static const int NULL_CODE{ 0 };

    inline static const int SPACE_CODE{ ' ' };

    inline static const int NEW_LINE_CODE{ '\n' };

    inline static const int FALLBACK_CODE{ '?' };

public:
    float GetSpaceWidth() const;

    float GetFontSizeScaledSpaceWidth(const float fontSize) const;

    std::shared_ptr<prev::render::buffer::ImageBuffer> GetImageBuffer() const;

    bool GetCharacter(const int charCode, Character& outCharacter) const;

private:
    friend class FontMetadataFactory;

private:
    float m_spaceWidth{};

    std::map<int, Character> m_characterMetaData;

    std::shared_ptr<prev::render::buffer::ImageBuffer> m_imageBuffer{};
};
} // namespace prev_test::render::font

#endif // !__FONT_METADATA_H__
