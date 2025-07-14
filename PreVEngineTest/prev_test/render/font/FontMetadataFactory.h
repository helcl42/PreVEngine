#ifndef __FONT_METADATA_FACTORY_H__
#define __FONT_METADATA_FACTORY_H__

#include "FontMetadata.h"
#include "FontMetadataFile.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/render/buffer/ImageBuffer.h>
#include <prev/render/sampler/Sampler.h>

#include <map>
#include <vector>

namespace prev_test::render::font {
class FontMetadataFactory final {
public:
    FontMetadataFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator);

    ~FontMetadataFactory() = default;

public:
    std::unique_ptr<FontMetadata> CreateFontMetadata(const std::string& metadataFilePath, const std::string& textureFilePath, const float aspectRatio = 16.0f / 9.0f, const int desiredPadding = 0) const;

private:
    struct FontMetadataState {
        float aspectRatio{};

        float verticalPerPixelSize{};

        float horizontalPerPixelSize{};

        std::vector<int> padding;

        int paddingWidth{};

        int paddingHeight{};

        float spaceWidth{};

        int imageWidth{};

        int imageHeight{};

        int desiredPadding{};
    };

    void ExtractPaddingData(FontMetadataFile& metaDataFile, FontMetadataState& state) const;

    void ExtractMeasureInfo(FontMetadataFile& metaDataFile, FontMetadataState& state) const;

    std::shared_ptr<prev::render::buffer::ImageBuffer> CreateImageBuffer(const std::string& textureFilePath) const;

    void ExtractCharactersData(FontMetadataFile& metaDataFile, FontMetadataState& state, std::map<int, Character>& characters) const;

    Character CreateSingleCharacter(FontMetadataFile& metaDataFile, const FontMetadataState& state, const int charCode) const;

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

private:
    inline static const int PADDING_TOP_INDEX{ 0 };
    inline static const int PADDING_LEFT_INDEX{ 1 };
    inline static const int PADDING_BOTTOM_INDEX{ 2 };
    inline static const int PADDING_RIGHT_INDEX{ 3 };
};

} // namespace prev_test::render::font

#endif // !__FONT_METADATA_FACTORY_H__
