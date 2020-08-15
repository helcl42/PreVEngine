#ifndef __IMAGE_FACTORY_H__
#define __IMAGE_FACTORY_H__

#include "Image.h"
#include "PixelRGBA.h"

#include <memory>
#include <string>

namespace prev {
class ImageFactory {
private:
    static bool FileExists(const std::string& fileName);

public:
    std::unique_ptr<Image> CreateImage(const std::string& filename, bool flipVertically = false) const;

    std::unique_ptr<Image> CreateImageWithPattern(const uint32_t width, const uint32_t height, const bool gradient = true, const int checkers = 64) const;

    std::unique_ptr<Image> CreateImageWithColor(const uint32_t width, const uint32_t height, const PixelRGBA& color) const;
};
} // namespace prev

#endif // !__IMAGE_FACTORY_H__
