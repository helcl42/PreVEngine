#include "FontRenderComponent.h"

#include "../../render/font/TextMeshFactory.h"
#include "../../render/model/ModelFactory.h"

#include <prev/core/AllocatorProvider.h>

namespace prev_test::component::font {
FontRenderComponent::FontRenderComponent(const std::shared_ptr<prev_test::render::font::FontMetadata>& fontMetaData)
    : m_fontMetaData(fontMetaData)
{
}

const std::map<uint32_t, RenderableText>& FontRenderComponent::GetRenderableTexts() const
{
    return m_renderableTexts;
}

std::shared_ptr<prev_test::render::font::FontMetadata> FontRenderComponent::GetFontMetadata() const
{
    return m_fontMetaData;
}

void FontRenderComponent::AddText(const uint32_t key, const std::shared_ptr<prev_test::render::font::ScreenSpaceText>& text)
{
    std::shared_ptr<prev_test::render::IMesh> mesh{ prev_test::render::font::TextMeshFactory{}.CreateTextMesh(text, m_fontMetaData) };

    auto renderableTextIter{ m_renderableTexts.find(key) };
    if (renderableTextIter == m_renderableTexts.cend()) {
        const uint32_t MaxVertexCount{ 1000 };
        const uint32_t MaxIndexCount{ 2000 };
        auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

        RenderableText renderableText{};
        for (uint32_t i = 0; i < RenderableText::ModelCount; ++i) {
            renderableText.models[i] = prev_test::render::model::ModelFactory{}.CreateHostVisible(mesh, MaxVertexCount, MaxIndexCount, *allocator);
        }
        renderableText.model = renderableText.models[renderableText.modelIndex];
        renderableText.text = text;

        m_renderableTexts[key] = renderableText;
    } else {
        RenderableText& currentRenderableText{ renderableTextIter->second };

        ++currentRenderableText.modelIndex;

        const auto& newModel{ currentRenderableText.models[currentRenderableText.modelIndex] };
        auto indexBuffer{ newModel->GetIndexBuffer() };
        auto vertexBuffer{ newModel->GetVertexBuffer() };
        newModel->GetVertexBuffer()->Data(mesh->GetVertexData(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());
        indexBuffer->Data(mesh->GetIndices().data(), static_cast<uint32_t>(mesh->GetIndices().size()));

        currentRenderableText.models[currentRenderableText.modelIndex] = prev_test::render::model::ModelFactory{}.Create(mesh, vertexBuffer, indexBuffer);
        currentRenderableText.model = currentRenderableText.models[currentRenderableText.modelIndex];
        currentRenderableText.text = text;
    }
}

void FontRenderComponent::RemoveText(const uint32_t key)
{
    m_renderableTexts.erase(key);
}

void FontRenderComponent::Reset()
{
    m_renderableTexts.clear();
}

/////////////////////////////////////////
// Font 3D
/////////////////////////////////////////
Font3dRenderComponent::Font3dRenderComponent(const std::shared_ptr<prev_test::render::font::FontMetadata>& fontMetaData)
    : m_fontMetaData(fontMetaData)
{
}

const std::map<uint32_t, RenderableText3d>& Font3dRenderComponent::GetRenderableTexts() const
{
    return m_renderableTexts;
}

std::shared_ptr<prev_test::render::font::FontMetadata> Font3dRenderComponent::GetFontMetadata() const
{
    return m_fontMetaData;
}

void Font3dRenderComponent::AddText(const uint32_t key, const std::shared_ptr<prev_test::render::font::WorldSpaceText>& text)
{
    std::shared_ptr<prev_test::render::IMesh> mesh{ prev_test::render::font::TextMeshFactory{}.CreateTextMesh(text, m_fontMetaData) };

    auto renderableTextIter{ m_renderableTexts.find(key) };
    if (renderableTextIter == m_renderableTexts.cend()) {
        const uint32_t MaxVertexCount{ 1000 };
        const uint32_t MaxIndexCount{ 2000 };
        auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

        RenderableText3d renderableText{};
        for (uint32_t i = 0; i < RenderableText3d::ModelCount; ++i) {
            renderableText.models[i] = prev_test::render::model::ModelFactory{}.CreateHostVisible(mesh, MaxVertexCount, MaxIndexCount, *allocator);
        }
        renderableText.model = renderableText.models[renderableText.modelIndex];
        renderableText.text = text;

        m_renderableTexts[key] = renderableText;
    } else {
        RenderableText3d& currentRenderableText{ renderableTextIter->second };

        ++currentRenderableText.modelIndex;

        const auto& newModel{ currentRenderableText.models[currentRenderableText.modelIndex] };
        auto indexBuffer{ newModel->GetIndexBuffer() };
        auto vertexBuffer{ newModel->GetVertexBuffer() };
        newModel->GetVertexBuffer()->Data(mesh->GetVertexData(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());
        indexBuffer->Data(mesh->GetIndices().data(), static_cast<uint32_t>(mesh->GetIndices().size()));

        currentRenderableText.models[currentRenderableText.modelIndex] = prev_test::render::model::ModelFactory{}.Create(mesh, vertexBuffer, indexBuffer);
        currentRenderableText.model = currentRenderableText.models[currentRenderableText.modelIndex];
        currentRenderableText.text = text;
    }
}

void Font3dRenderComponent::RemoveText(const uint32_t key)
{
    m_renderableTexts.erase(key);
}

void Font3dRenderComponent::Reset()
{
    m_renderableTexts.clear();
}

} // namespace prev_test::component::font