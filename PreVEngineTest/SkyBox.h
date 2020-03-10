#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include "Common.h"
#include "General.h"

class CubeMeshVerticesOnly : public IMesh {
public:
    const VertexLayout& GetVertextLayout() const override
    {
        return m_vertexLayout;
    }

    const void* GetVertices() const override
    {
        return (const void*)m_vertices.data();
    }

    uint32_t GerVerticesCount() const override
    {
        return static_cast<uint32_t>(m_vertices.size());
    }

    const std::vector<uint32_t>& GerIndices() const override
    {
        return m_indices;
    }

    bool HasIndices() const override
    {
        return m_indices.size() > 0;
    }

private:
    static const inline float SIZE = 1000.0f;

private:
    const VertexLayout m_vertexLayout{ { VertexLayoutComponent::VEC3 } };

    const std::vector<glm::vec3> m_vertices = {
        // FROMT
        { -0.5f, -0.5f, 0.5f },
        { 0.5f, -0.5f, 0.5f },
        { 0.5f, 0.5f, 0.5f },
        { -0.5f, 0.5f, 0.5f },

        // BACK
        { -0.5f, -0.5f, -0.5f },
        { 0.5f, -0.5f, -0.5f },
        { 0.5f, 0.5f, -0.5f },
        { -0.5f, 0.5f, -0.5f },

        // TOP
        { -0.5f, 0.5f, 0.5f },
        { 0.5f, 0.5f, 0.5f },
        { 0.5f, 0.5f, -0.5f },
        { -0.5f, 0.5f, -0.5f },

        // BOTTOM
        { -0.5f, -0.5f, 0.5f },
        { 0.5f, -0.5f, 0.5f },
        { 0.5f, -0.5f, -0.5f },
        { -0.5f, -0.5f, -0.5f },

        // LEFT
        { -0.5f, -0.5f, 0.5f },
        { -0.5f, 0.5f, 0.5f },
        { -0.5f, 0.5f, -0.5f },
        { -0.5f, -0.5f, -0.5f },

        // RIGHT
        { 0.5f, -0.5f, 0.5f },
        { 0.5f, 0.5f, 0.5f },
        { 0.5f, 0.5f, -0.5f },
        { 0.5f, -0.5f, -0.5f }
    };

    const std::vector<uint32_t> m_indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };
};

class ISkyBoxComponent {
public:
    virtual std::shared_ptr<IModel> GetModel() const = 0;

    virtual std::shared_ptr<IMaterial> GetMaterial() const = 0;

public:
    virtual ~ISkyBoxComponent() = default;
};

class SkyBoxComponentFactory;

class SkyBoxComponent : public ISkyBoxComponent {
public:
    std::shared_ptr<IModel> GetModel() const override
    {
        return m_model;
    }

    std::shared_ptr<IMaterial> GetMaterial() const override
    {
        return m_material;
    }

private:
    friend class SkyBoxComponentFactory;

private:
    std::shared_ptr<IModel> m_model;

    std::shared_ptr<IMaterial> m_material;
};

class SkyBoxComponentFactory {
public:
    std::unique_ptr<ISkyBoxComponent> Create() const
    {
        const std::vector<std::string> materialPaths = {
            "SkyBoxes/Sky/left.png",
            "SkyBoxes/Sky/right.png",
            "SkyBoxes/Sky/top.png",
            "SkyBoxes/Sky/top.png",
            "SkyBoxes/Sky/front.png",
            "SkyBoxes/Sky/back.png",
        };

        auto allocator = AllocatorProvider::GetInstance().GetAllocator();

        auto skyBox = std::make_unique<SkyBoxComponent>();
        skyBox->m_model = CreateModel(*allocator);
        skyBox->m_material = CreateMaterial(*allocator, materialPaths);
        return skyBox;
    }

private:
    std::unique_ptr<IModel> CreateModel(Allocator& allocator) const
    {
        auto mesh = std::make_unique<CubeMeshVerticesOnly>();
        auto vertexBuffer = std::make_unique<VBO>(allocator);
        vertexBuffer->Data(mesh->GetVertices(), mesh->GerVerticesCount(), mesh->GetVertextLayout().GetStride());
        auto indexBuffer = std::make_unique<IBO>(allocator);
        indexBuffer->Data(mesh->GerIndices().data(), static_cast<uint32_t>(mesh->GerIndices().size()));
        return std::make_unique<Model>(std::move(mesh), std::move(vertexBuffer), std::move(indexBuffer));
    }

    std::unique_ptr<IMaterial> CreateMaterial(Allocator& allocator, const std::vector<std::string>& textureFilenames) const
    {
        std::vector<std::shared_ptr<Image> > images;
        ImageFactory imageFactory{};
        for (size_t i = 0; i < textureFilenames.size(); i++) {
            auto image = imageFactory.CreateImage(textureFilenames[i]);
            images.emplace_back(std::move(image));
        }

        const uint32_t imageSizeInBytes = static_cast<uint32_t>(images[0]->GetWidth() * images[0]->GetHeight() * sizeof(RGBA));
        const uint32_t totalSizeInBytes = static_cast<uint32_t>(imageSizeInBytes * images.size());
        auto data = std::shared_ptr<uint8_t[]>(new uint8_t[totalSizeInBytes]);
        for (size_t i = 0; i < textureFilenames.size(); i++) {
            std::memcpy(&data[i * imageSizeInBytes], images[i]->GetBuffer().get(), imageSizeInBytes);
        }

        auto imageBuffer = std::make_unique<ImageBuffer>(allocator);
        imageBuffer->Create(ImageBufferCreateInfo{ { images[0]->GetWidth(), images[0]->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, true, VK_IMAGE_VIEW_TYPE_CUBE, static_cast<uint32_t>(images.size()), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER, data.get() });

        return std::make_unique<Material>(std::move(images[0]), std::move(imageBuffer), 1.0f, 0.0f);
    }
};

#endif // !__SKYBOX_H__
