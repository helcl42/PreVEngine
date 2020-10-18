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
    prev_test::render::font::TextMeshFactory meshFactory;
    auto mesh = meshFactory.CreateTextMesh(text, m_fontMetaData);

    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();
    auto vertexBuffer = std::make_unique<prev::core::memory::buffer::VertexBuffer>(*allocator);
    vertexBuffer->Data(mesh->GetVertexData(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());

    auto indexBuffer = std::make_unique<prev::core::memory::buffer::IndexBuffer>(*allocator);
    indexBuffer->Data(mesh->GetIndices().data(), (uint32_t)mesh->GetIndices().size());

    prev_test::render::model::ModelFactory modelFactory{};
    std::shared_ptr<prev_test::render::IModel> model = modelFactory.Create(std::move(mesh), std::move(vertexBuffer), std::move(indexBuffer));

    m_renderableTexts.push_back(RenderableText{ text, model });
}

void DefaultFontRenderComponent::Reset()
{
    m_renderableTexts.clear();
}
} // namespace prev_test::component::font