#include "ImageFactory.h"

#include "../../common/Logger.h"
#include "../../util/Utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include <external/stb_image.h>

namespace prev::render::image {
std::unique_ptr<Image> ImageFactory::CreateImage(const std::string& filename, bool flipVertically) const
{
    if (!prev::util::file::Exists(filename)) {
        LOGE("Image: File not found: %s\n", filename.c_str());
        return nullptr;
    }

    LOGI("Loading image: %s...\n", filename.c_str());

    stbi_set_flip_vertically_on_load(flipVertically);
#ifdef TARGET_PLATFORM_IOS
    stbi_convert_iphone_png_to_rgb(true);
#endif
    
    int w, h, c;
    uint8_t* imageBytes = reinterpret_cast<uint8_t*>(stbi_load(filename.c_str(), &w, &h, &c, STBI_rgb_alpha));
    if (!imageBytes) {
        LOGE("Image: Failed to load texture: %s", filename.c_str());
        return nullptr;
    }

    auto image = std::make_unique<Image>(w, h, imageBytes);

    LOGI("Loaded image: %s (%dx%d)\n", filename.c_str(), w, h);

    stbi_image_free(imageBytes);

    return image;
}

std::unique_ptr<Image> ImageFactory::CreateImageFromMemory(const uint8_t* data, const int dataLength) const
{
    LOGI("Loading image from memory: %d bytes\n", dataLength);

#ifdef TARGET_PLATFORM_IOS
    stbi_convert_iphone_png_to_rgb(true);
#endif
    
    int w, h, c;
    uint8_t* imageBytes = reinterpret_cast<uint8_t*>(stbi_load_from_memory(data, dataLength, &w, &h, &c, STBI_rgb_alpha));
    if (!imageBytes) {
        LOGE("Image: Failed to load texture from memory");
        return nullptr;
    }

    auto image = std::make_unique<Image>(w, h, imageBytes);

    LOGI("Loaded image from memory: %d bytes (%dx%d)\n", dataLength, w, h);

    stbi_image_free(imageBytes);

    return image;
}

std::unique_ptr<Image> ImageFactory::CreateImageWithPattern(const uint32_t width, const uint32_t height, const bool gradient, const int checkers) const
{
    auto image = std::make_unique<Image>(width, height);
    if (!image) {
        return nullptr;
    }

    auto buffer = image->GetBuffer();

    for (uint32_t y = 0; y < image->GetHeight(); ++y) {
        for (uint32_t x = 0; x < image->GetWidth(); ++x) {
            auto& pix{ buffer[y * width + x] };

            pix = { 0, 0, 0, 255 };

            if (gradient) {
                pix = { static_cast<uint8_t>(x), static_cast<uint8_t>(y), 0, 255 };
            }

            if (checkers) {
                if ((x / (width / checkers)) % 2 == (y / (height / checkers)) % 2) {
                    pix.B = 128;
                }
            }
        }
    }

    return image;
}

std::unique_ptr<Image> ImageFactory::CreateImageWithColor(const uint32_t width, const uint32_t height, const PixelRGBA& color) const
{
    auto image = std::make_unique<Image>(width, height);
    if (!image) {
        return nullptr;
    }

    image->Clear(color);

    return image;
}
} // namespace prev::render::image
