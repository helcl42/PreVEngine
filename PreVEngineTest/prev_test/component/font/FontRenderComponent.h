#ifndef __FONT_RENDER_COMPONENT_H__
#define __FONT_RENDER_COMPONENT_H__

#include "IFontRenderComponent.h"

#include "../../render/font/TextMeshFactory.h"
#include "../../render/model/ModelFactory.h"

#include <prev/core/AllocatorProvider.h>

#include <map>

namespace prev_test::component::font {
template <typename TextType>
class FontRenderComponent : public IFontRenderComponent<TextType> {
public:
    FontRenderComponent(const std::shared_ptr<prev_test::render::font::FontMetadata>& fontMetaData)
        : m_fontMetaData(fontMetaData)
    {
    }

    ~FontRenderComponent() = default;

public:
    const std::map<uint32_t, RenderableText<TextType>>& GetRenderableTexts() const override
    {
        return m_renderableTexts;
    }

    std::shared_ptr<prev_test::render::font::FontMetadata> GetFontMetadata() const override
    {
        return m_fontMetaData;
    }

    void AddText(const uint32_t key, const std::shared_ptr<TextType>& text) override
    {
        std::shared_ptr<prev_test::render::IMesh> mesh{ prev_test::render::font::TextMeshFactory{}.CreateTextMesh(text, m_fontMetaData) };

        auto renderableTextIter{ m_renderableTexts.find(key) };
        if (renderableTextIter == m_renderableTexts.cend()) {
            const uint32_t MaxVertexCount{ 1000 };
            const uint32_t MaxIndexCount{ 2000 };
            auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

            RenderableText<TextType> renderableText{};
            for (uint32_t i = 0; i < RenderableText<TextType>::ModelCount; ++i) {
                renderableText.models[i] = prev_test::render::model::ModelFactory{}.CreateHostVisible(mesh, MaxVertexCount, MaxIndexCount, *allocator);
            }
            renderableText.model = renderableText.models[renderableText.modelIndex];
            renderableText.text = text;

            m_renderableTexts[key] = renderableText;
        } else {
            RenderableText<TextType>& currentRenderableText{ renderableTextIter->second };

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

    void RemoveText(const uint32_t key) override
    {
        m_renderableTexts.erase(key);
    }

    void Reset() override
    {
        m_renderableTexts.clear();
    }

private:
    std::shared_ptr<prev_test::render::font::FontMetadata> m_fontMetaData;

    std::map<uint32_t, RenderableText<TextType>> m_renderableTexts;
};
} // namespace prev_test::component::font

#endif // !__FONT_RENDER_COMPONENT_H__
