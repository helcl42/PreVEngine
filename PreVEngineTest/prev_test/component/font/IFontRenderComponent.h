#ifndef __IFONT_RENDER_COMPONENT_H__
#define __IFONT_RENDER_COMPONENT_H__

#include "../../render/IModel.h"
#include "../../render/font/FontMetadata.h"

#include <prev/util/Utils.h>

#include <map>

namespace prev_test::component::font {
template <typename TextType>
struct RenderableText {
    static constexpr inline uint32_t ModelCount{ 2 };

    std::shared_ptr<TextType> text{};

    std::shared_ptr<prev_test::render::IModel> models[ModelCount] = {};

    std::shared_ptr<prev_test::render::IModel> model{};

    prev::util::CircularIndex<uint32_t> modelIndex{ ModelCount };
};

template <typename TextType>
class IFontRenderComponent {
public:
    virtual std::shared_ptr<prev_test::render::font::FontMetadata> GetFontMetadata() const = 0;

    virtual const std::map<uint32_t, RenderableText<TextType>>& GetRenderableTexts() const = 0;

    virtual void AddText(const uint32_t key, const std::shared_ptr<TextType>& text) = 0;

    virtual void RemoveText(const uint32_t key) = 0;

    virtual void Reset() = 0;

public:
    virtual ~IFontRenderComponent() = default;
};
} // namespace prev_test::component::font

#endif // !__IFONT_RENDER_COMPONENT_H__
