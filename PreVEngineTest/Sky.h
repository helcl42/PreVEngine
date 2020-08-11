#ifndef __SKY_H__
#define __SKY_H__

#include "General.h"

static const float SKY_BOX_SIZE = 300.0f;

class CubeMeshVerticesOnly : public IMesh {
public:
    const VertexLayout& GetVertexLayout() const override
    {
        return m_vertexLayout;
    }

    const void* GetVertexData() const override
    {
        return (const void*)vertices.data();
    }

    const std::vector<glm::vec3>& GetVertices() const override
    {
        return vertices;
    }

    uint32_t GerVerticesCount() const override
    {
        return static_cast<uint32_t>(vertices.size());
    }

    const std::vector<uint32_t>& GetIndices() const override
    {
        return indices;
    }

    const std::vector<MeshPart>& GetMeshParts() const override
    {
        return meshParts;
    }

private:
    static const inline VertexLayout m_vertexLayout{ { VertexLayoutComponent::VEC3 } };

    static const inline std::vector<glm::vec3> vertices = {
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

    static const inline std::vector<uint32_t> indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };

    static const inline std::vector<MeshPart> meshParts = {
        MeshPart(static_cast<uint32_t>(indices.size()))
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
            AssetManager::Instance().GetAssetPath("SkyBoxes/Sky/right.png"),
            AssetManager::Instance().GetAssetPath("SkyBoxes/Sky/left.png"),
            AssetManager::Instance().GetAssetPath("SkyBoxes/Sky/top.png"),
            AssetManager::Instance().GetAssetPath("SkyBoxes/Sky/top.png"),
            AssetManager::Instance().GetAssetPath("SkyBoxes/Sky/back.png"),
            AssetManager::Instance().GetAssetPath("SkyBoxes/Sky/front.png"),
        };

        auto allocator = AllocatorProvider::Instance().GetAllocator();

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
        vertexBuffer->Data(mesh->GetVertexData(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());
        auto indexBuffer = std::make_unique<IBO>(allocator);
        indexBuffer->Data(mesh->GetIndices().data(), static_cast<uint32_t>(mesh->GetIndices().size()));
        return std::make_unique<Model>(std::move(mesh), std::move(vertexBuffer), std::move(indexBuffer));
    }

    std::unique_ptr<IMaterial> CreateMaterial(Allocator& allocator, const std::vector<std::string>& textureFilenames) const
    {
        ImageFactory imageFactory{};

        std::vector<std::shared_ptr<Image> > images{};
        for (const auto& faceFilePath : textureFilenames) {
            auto image = imageFactory.CreateImage(faceFilePath);
            images.emplace_back(std::move(image));
        }

        std::vector<const uint8_t*> layersData{};
        for (const auto& image : images) {
            layersData.emplace_back((const uint8_t*)image->GetBuffer());
        }

        auto imageBuffer = std::make_unique<ImageBuffer>(allocator);
        imageBuffer->Create(ImageBufferCreateInfo{ VkExtent2D{ images[0]->GetWidth(), images[0]->GetHeight() }, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, true, true, VK_IMAGE_VIEW_TYPE_CUBE, static_cast<uint32_t>(images.size()), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, layersData });

        return std::make_unique<Material>(std::move(images[0]), std::move(imageBuffer), 1.0f, 0.0f);
    }
};

class ISkyComponent {
public:
    virtual std::shared_ptr<IModel> GetModel() const = 0;

    virtual glm::vec3 GetBottomColor() const = 0;

    virtual glm::vec3 GetTopColor() const = 0;

public:
    virtual ~ISkyComponent() = default;
};

class SkyComponentFactory;

class SkyComponent : public ISkyComponent {
public:
    std::shared_ptr<IModel> GetModel() const override
    {
        return m_model;
    }

    glm::vec3 GetBottomColor() const override
    {
        return m_bottomColor;
    }

    glm::vec3 GetTopColor() const override
    {
        return m_topColor;
    }

private:
    friend class SkyComponentFactory;

private:
    std::shared_ptr<IModel> m_model;

    glm::vec3 m_bottomColor;

    glm::vec3 m_topColor;
};

class SkyComponentFactory {
public:
    std::unique_ptr<ISkyComponent> Create() const
    {
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        auto sky = std::make_unique<SkyComponent>();
        sky->m_model = CreateModel(*allocator);
        sky->m_bottomColor = glm::vec3(FOG_COLOR.x, FOG_COLOR.y, FOG_COLOR.z);
        sky->m_topColor = glm::vec3(0.521f, 0.619f, 0.839);
        return sky;
    }

private:
    std::unique_ptr<IModel> CreateModel(Allocator& allocator) const
    {
        auto mesh = std::make_unique<FullScreenQuadMesh>();
        auto vertexBuffer = std::make_unique<VBO>(allocator);
        vertexBuffer->Data(mesh->GetVertexData(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());
        auto indexBuffer = std::make_unique<IBO>(allocator);
        indexBuffer->Data(mesh->GetIndices().data(), static_cast<uint32_t>(mesh->GetIndices().size()));
        return std::make_unique<Model>(std::move(mesh), std::move(vertexBuffer), std::move(indexBuffer));
    }
};


#endif // !__SKY_H__

