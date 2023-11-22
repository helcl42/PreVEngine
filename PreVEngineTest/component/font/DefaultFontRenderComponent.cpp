#include "DefaultFontRenderComponent.h"

#include "../../render/font/TextMeshFactory.h"
#include "../../render/model/ModelFactory.h"

#include <prev/core/AllocatorProvider.h>

namespace prev_test::component::font {
DefaultFontRenderComponent::DefaultFontRenderComponent(const std::shared_ptr<prev_test::render::font::FontMetadata>& fontMetaData)
    : m_fontMetaData(fontMetaData)
{
}

const std::map<uint32_t, RenderableText>& DefaultFontRenderComponent::GetRenderableTexts() const
{
    return m_renderableTexts;
}

std::shared_ptr<prev_test::render::font::FontMetadata> DefaultFontRenderComponent::GetFontMetadata() const
{
    return m_fontMetaData;
}

void DefaultFontRenderComponent::AddText(const uint32_t key, const std::shared_ptr<prev_test::render::font::ScreenSpaceText>& text)
{
    std::shared_ptr<prev_test::render::IMesh> mesh{ prev_test::render::font::TextMeshFactory{}.CreateTextMesh(text, m_fontMetaData) };

    auto renderableTextIter{ m_renderableTexts.find(key) };
    if (renderableTextIter == m_renderableTexts.cend()) {
        const uint32_t MaxVertexCount{ 1000 };
        const uint32_t MaxIndexCount{ 2000 };
        const uint32_t InitialIndex{ 0 };
        auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

        RenderableText renderableText{};
        renderableText.modelIndex = InitialIndex;
        for (uint32_t i = 0; i < RenderableText::ModelCount; ++i) {
            renderableText.models[i] = prev_test::render::model::ModelFactory{}.CreateHostVisible(mesh, MaxVertexCount, MaxIndexCount, *allocator);
        }
        renderableText.model = renderableText.models[renderableText.modelIndex];
        renderableText.text = text;

        m_renderableTexts[key] = renderableText;
    } else {
        RenderableText& currentRenderableText{ renderableTextIter->second };

        const uint32_t newIndex{ (currentRenderableText.modelIndex + 1) % RenderableText::ModelCount };

        const auto& newModel{ currentRenderableText.models[newIndex] };
        auto indexBuffer{ newModel->GetIndexBuffer() };
        auto vertexBuffer{ newModel->GetVertexBuffer() };
        newModel->GetVertexBuffer()->Data(mesh->GetVertexData(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());
        indexBuffer->Data(mesh->GetIndices().data(), static_cast<uint32_t>(mesh->GetIndices().size()));

        currentRenderableText.modelIndex = newIndex;
        currentRenderableText.models[newIndex] = prev_test::render::model::ModelFactory{}.Create(mesh, vertexBuffer, indexBuffer);
        currentRenderableText.model = currentRenderableText.models[newIndex];
        currentRenderableText.text = text;
    }
}

void DefaultFontRenderComponent::RemoveText(const uint32_t key)
{
    m_renderableTexts.erase(key);
}

void DefaultFontRenderComponent::Reset()
{
    m_renderableTexts.clear();
}
} // namespace prev_test::component::font