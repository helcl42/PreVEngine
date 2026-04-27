#ifndef __IMAGE_BUFFER_VIEW_BUILDER_H__
#define __IMAGE_BUFFER_VIEW_BUILDER_H__

#include "ImageBufferView.h"
#include "ImageBuffer.h"

#include <memory>

namespace prev::render::buffer {

class ImageBufferViewBuilder final {
public:
    ImageBufferViewBuilder(const ImageBuffer& imageBuffer);

    ~ImageBufferViewBuilder() = default;

public:
    ImageBufferViewBuilder& SetBaseMipLevel(uint32_t baseMipLevel);

    ImageBufferViewBuilder& SetMipLevelCount(uint32_t mipLevelCount);

    ImageBufferViewBuilder& SetBaseArrayLayer(uint32_t baseArrayLayer);

    ImageBufferViewBuilder& SetArrayLayerCount(uint32_t arrayLayerCount);

    std::unique_ptr<ImageBufferView> Build() const;

private:
    const ImageBuffer& m_imageBuffer;

    uint32_t m_baseMipLevel{ 0 };

    uint32_t m_mipLevelCount{ 1 };

    uint32_t m_baseArrayLayer{ 0 };

    uint32_t m_arrayLayerCount{ 0 }; // 0 means use imageBuffer's layer count
};

} // namespace prev::render::buffer

#endif
