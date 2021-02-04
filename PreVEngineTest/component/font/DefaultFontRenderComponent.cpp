#include "DefaultFontRenderComponent.h"

#include "../../render/font/TextMeshFactory.h"
#include "../../render/model/ModelFactory.h"

#include <prev/scene/AllocatorProvider.h>

namespace prev_test::component::font {
DefaultFontRenderComponent::DefaultFontRenderComponent(const std::shared_ptr<prev_test::render::font::FontMetadata>& fontMetaData)
    : m_fontMetaData(fontMetaData)
{
}

const std::vector<RenderableText>& DefaultFontRenderComponent::GetRenderableTexts() const
{
    return m_renderableTexts;
}

std::shared_ptr<prev_test::render::font::FontMetadata> DefaultFontRenderComponent::GetFontMetadata() const
{
    return m_fontMetaData;
}

void DefaultFontRenderComponent::AddText(const std::shared_ptr<prev_test::render::font::ScreenSpaceText>& text)
{
    auto allocator{ prev::scene::AllocatorProvider::Instance().GetAllocator() };

    prev_test::render::font::TextMeshFactory meshFactory;
    auto mesh{ meshFactory.CreateTextMesh(text, m_fontMetaData) };

    prev_test::render::model::ModelFactory modelFactory{};
    auto model{ modelFactory.Create(std::move(mesh), *allocator) };

    m_renderableTexts.push_back(RenderableText{ text, std::move(model) });
}

void DefaultFontRenderComponent::Reset()
{
    m_renderableTexts.clear();
}
} // namespace prev_test::component::font