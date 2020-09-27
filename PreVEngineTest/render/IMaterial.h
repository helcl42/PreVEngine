#ifndef __IMATERIAL_H__
#define __IMATERIAL_H__

#include <prev/common/Common.h>
#include <prev/core/memory/image/IImageBuffer.h>
#include <prev/render/image/Image.h>

#include <memory>

namespace prev_test::render {
struct MaterialProperties {
    glm::vec3 color;
    float shineDamper;
    float reflectivity;
};

struct ImagePair {
    std::shared_ptr<prev::render::image::Image> image;
    std::shared_ptr<prev::core::memory::image::IImageBuffer> imageBuffer;
};

class IMaterial {
public:
    virtual std::shared_ptr<prev::render::image::Image> GetImage() const = 0;

    virtual std::shared_ptr<prev::core::memory::image::IImageBuffer> GetImageBuffer() const = 0;

    virtual bool HasImage() const = 0;

    virtual std::shared_ptr<prev::render::image::Image> GetNormalImage() const = 0;

    virtual std::shared_ptr<prev::core::memory::image::IImageBuffer> GetNormalmageBuffer() const = 0;

    virtual bool HasNormalImage() const = 0;

    virtual std::shared_ptr<prev::render::image::Image> GetHeightImage() const = 0;

    virtual std::shared_ptr<prev::core::memory::image::IImageBuffer> GetHeightImageBuffer() const = 0;

    virtual bool HasHeightImage() const = 0;

    virtual float GetShineDamper() const = 0;

    virtual float GetReflectivity() const = 0;

    virtual bool HasTransparency() const = 0;

    virtual void SetHasTransparency(bool transparency) = 0;

    virtual bool UsesFakeLightning() const = 0;

    virtual void SetUsesFakeLightning(bool fake) = 0;

    virtual unsigned int GetAtlasNumberOfRows() const = 0;

    virtual void SetAtlasNumberOfRows(unsigned int rows) = 0;

    virtual const glm::vec2& GetTextureOffset() const = 0;

    virtual void SetTextureOffset(const glm::vec2& textureOffset) = 0;

    virtual const glm::vec3& GetColor() const = 0;

    virtual float GetHeightScale() const = 0;

    virtual void SetHeightScale(const float scale) = 0;

public:
    virtual ~IMaterial() = default;
};
} // namespace prev_test::render

#endif // !__MATERIAL_H__
