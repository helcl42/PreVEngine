#ifndef __FONT_METADATA_FACTORY_H__
#define __FONT_METADATA_FACTORY_H__

#include "FontMetadata.h"

#include <prev/core/device/Device.h>
#include <prev/render/buffer/ImageBuffer.h>

#include <map>
#include <vector>

namespace prev_test::render::font {
class FontMetadataFactory final {
public:
    FontMetadataFactory(prev::core::device::Device& device);

    ~FontMetadataFactory() = default;

public:
    std::unique_ptr<FontMetadata> CreateFontMetadata(const std::string& metadataFilePath, const std::string& textureFilePath, const float aspectRatio, const float lineHeight, const int extraPadding = 0) const;

private:
    prev::core::device::Device& m_device;
};

} // namespace prev_test::render::font

#endif // !__FONT_METADATA_FACTORY_H__
