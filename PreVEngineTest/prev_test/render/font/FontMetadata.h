#ifndef __FONT_METADATA_H__
#define __FONT_METADATA_H__

#include "Character.h"

#include <prev/render/buffer/ImageBuffer.h>

#include <map>
#include <memory>

namespace prev_test::render::font {
class FontMetadata {
public:
    FontMetadata(const std::shared_ptr<prev::render::buffer::ImageBuffer>& imageBuffer, const std::map<int, Character>& characterMetaData, const float spaceWidth, const float lineHeight);

    ~FontMetadata() = default;

public:
    float GetSpaceWidth(const float fontSize) const;

    float GetLineHeight(const float fontSize) const;

    std::shared_ptr<prev::render::buffer::ImageBuffer> GetImageBuffer() const;

    bool GetCharacter(const int charCode, Character& outCharacter) const;

public:
    inline static const int NULL_CHARACTER{ 0 };

    inline static const int SPACE_CHARACTER{ ' ' };

    inline static const int NEW_LINE_CHARACTER{ '\n' };

    inline static const int UNKNOWN_CHARACTER{ '?' };

private:
    std::shared_ptr<prev::render::buffer::ImageBuffer> m_imageBuffer{};

    std::map<int, Character> m_characterMetaData;

    float m_spaceWidth{};

    float m_lineHeight{};
};
} // namespace prev_test::render::font

#endif // !__FONT_METADATA_H__
