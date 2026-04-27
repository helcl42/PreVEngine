#ifndef __IMAGE_BUFFER_VIEW_H__
#define __IMAGE_BUFFER_VIEW_H__

#include <gfx/gfx.h>

namespace prev::render::buffer {

class ImageBufferViewBuilder;

class ImageBufferView final {
public:
    ImageBufferView(GfxTextureView view);

    ~ImageBufferView();

    ImageBufferView(const ImageBufferView&) = delete;

    ImageBufferView& operator=(const ImageBufferView&) = delete;

    ImageBufferView(ImageBufferView&& other) noexcept;

    ImageBufferView& operator=(ImageBufferView&& other) noexcept;

public:
    operator GfxTextureView() const;

public:
    friend class ImageBufferViewBuilder;

private:
    GfxTextureView m_view{};
};

} // namespace prev::render::buffer

#endif
