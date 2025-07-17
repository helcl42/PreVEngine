#ifndef __IMAGE_FACTORY_H__
#define __IMAGE_FACTORY_H__

#include "IImage.h"
#include "Pixel.h"

#include <memory>
#include <string>

namespace prev::render::image {
class ImageFactory final {
public:
    std::unique_ptr<IImage> CreateImage(const std::string& filename, bool flipVertically = false) const;

    std::unique_ptr<IImage> CreateImageFromMemory(const uint8_t* data, const uint32_t dataLength) const;

    std::unique_ptr<IImage> CreateImageWithPattern(const uint32_t width, const uint32_t height, const bool gradient = true, const uint32_t checkers = 64) const;

    std::unique_ptr<IImage> CreateImageWithColor(const uint32_t width, const uint32_t height, const Pixel<uint8_t, 4>& color) const;
};
} // namespace prev::render::image

#endif // !__IMAGE_FACTORY_H__
