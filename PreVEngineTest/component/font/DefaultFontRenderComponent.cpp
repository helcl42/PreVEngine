#include "DefaultFontRenderComponent.h"

#include "../../render/font/TextMeshFactory.h"
#include "../../render/model/Model.h"

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
    prev_test::render::font::TextMeshFactory meshFactory;
    auto mesh = meshFactory.CreateTextMesh(text, m_fontMetaData);

    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();
    auto vertexBuffer = std::make_shared<prev::core::memory::buffer::VertexBuffer>(*allocator);
    vertexBuffer->Data(mesh->GetVertexData(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());

    auto indexBuffer = std::make_shared<prev::core::memory::buffer::IndexBuffer>(*allocator);
    indexBuffer->Data(mesh->GetIndices().data(), (uint32_t)mesh->GetIndices().size());

    auto model = std::make_shared<prev_test::render::model::Model>(std::move(mesh), vertexBuffer, indexBuffer);

    m_renderableTexts.push_back(RenderableText{ text, model });
}

void DefaultFontRenderComponent::Reset()
{
    m_renderableTexts.clear();
}
} // namespace prev_test::component::font