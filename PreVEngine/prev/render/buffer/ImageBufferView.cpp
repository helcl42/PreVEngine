#include "ImageBufferView.h"

namespace prev::render::buffer {

ImageBufferView::ImageBufferView(GfxTextureView view)
    : m_view{ view }
{
}

ImageBufferView::~ImageBufferView()
{
    if (m_view) {
        gfxTextureViewDestroy(m_view);
    }
}

ImageBufferView::ImageBufferView(ImageBufferView&& other) noexcept
    : m_view{ other.m_view }
{
    other.m_view = nullptr;
}

ImageBufferView& ImageBufferView::operator=(ImageBufferView&& other) noexcept
{
    if (this != &other) {
        if (m_view) {
            gfxTextureViewDestroy(m_view);
        }
        m_view = other.m_view;
        other.m_view = nullptr;
    }
    return *this;
}

ImageBufferView::operator GfxTextureView() const
{
    return m_view;
}

} // namespace prev::render::buffer
