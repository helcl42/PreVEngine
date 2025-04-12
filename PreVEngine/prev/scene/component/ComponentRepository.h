#ifndef __COMPONENT_REPOSITORY_H__
#define __COMPONENT_REPOSITORY_H__

#include "IComponent.h"

#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <typeindex>
#include <vector>

namespace prev::scene::component {
class ComponentRepository final {
public:
    ComponentRepository() = default;

    ~ComponentRepository() = default;

public:
    template <typename ComponentType>
    std::shared_ptr<ComponentType> Get() const
    {
        const auto result{ FindOne<ComponentType>() };
        if (!result) {
            throw std::runtime_error("Trying get component with tag = " + GetTypeName<ComponentType>() + " that does not exist in this repository.");
        }
        return result;
    }

    template <typename ComponentType>
    std::vector<std::shared_ptr<ComponentType>> GetAll() const
    {
        const auto result{ FindAll<ComponentType>() };
        if (result.empty()) {
            throw std::runtime_error("Trying to get components with tag = " + GetTypeName<ComponentType>() + " that does not exist in this repository.");
        }
        return result;
    }

    template <typename ComponentType>
    void Add(const std::shared_ptr<ComponentType>& component)
    {
        auto& currentComponents{ m_components[GetTypeIndex<ComponentType>()] };
        currentComponents.push_back(component);
    }

    template <typename ComponentType>
    void Add(const std::vector<std::shared_ptr<IComponent>>& components)
    {
        auto& currentComponents{ m_components[GetTypeIndex<ComponentType>()] };
        currentComponents.insert(currentComponents.end(), components.cbegin(), components.cend());
    }

    template <typename ComponentType>
    void Remove()
    {
        if (!Contains<ComponentType>()) {
            throw std::runtime_error("Trying to remove component with tag = " + GetTypeName<ComponentType>() + " that does not exist in this repository.");
        }
        m_components.erase(GetTypeIndex<ComponentType>());
    }

    template <typename ComponentType>
    bool Contains() const
    {
        return m_components.find(GetTypeIndex<ComponentType>()) != m_components.cend();
    }

    template <typename ComponentType>
    std::shared_ptr<ComponentType> FindOne() const
    {
        auto iter{ m_components.find(GetTypeIndex<ComponentType>()) };
        if (iter == m_components.cend()) {
            return {};
        }
        return Cast<ComponentType>(iter->second.front());
    }

    template <typename ComponentType>
    std::vector<std::shared_ptr<ComponentType>> FindAll() const
    {
        auto iter{ m_components.find(GetTypeIndex<ComponentType>()) };
        if (iter == m_components.cend()) {
            return {};
        }

        std::vector<std::shared_ptr<ComponentType>> result;
        for (size_t i = 0; i < iter->second.size(); ++i) {
            const auto component{ Cast<ComponentType>(iter->second[i]) };
            if (component) {
                result.push_back(component);
            }
        }
        return result;
    }

private:
    template <typename ComponentType>
    static inline std::shared_ptr<ComponentType> Cast(const std::shared_ptr<IComponent>& component)
    {
        // TODO - static cast should be enough here - enable this in debug only ???
        // return std::dynamic_pointer_cast<ComponentType>(component);
        return std::static_pointer_cast<ComponentType>(component);
    }

    template <typename ComponentType>
    static inline std::type_index GetTypeIndex()
    {
        return std::type_index(typeid(ComponentType));
    }

    template <typename ComponentType>
    static inline std::string GetTypeName()
    {
        return std::string(GetTypeIndex<ComponentType>().name());
    }

private:
    std::unordered_map<std::type_index, std::vector<std::shared_ptr<IComponent>>> m_components;
};
} // namespace prev::scene::component

#endif