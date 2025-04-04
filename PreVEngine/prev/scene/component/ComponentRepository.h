#ifndef __COMPONENT_REPOSITORY_H__
#define __COMPONENT_REPOSITORY_H__

#include "../../common/Logger.h"
#include "../../common/pattern/Singleton.h"
#include "../SceneEvents.h"

#include <prev/event/EventHandler.h>

#include <map>
#include <memory>
#include <vector>

namespace prev::scene::component {
template <typename ItemType>
class ComponentRepository final : public prev::common::pattern::Singleton<ComponentRepository<ItemType>> {
public:
    ~ComponentRepository()
    {
        for (const auto& [nodeId, components] : m_components) {
            LOGW("ComponentRepository<%s>: For Node Id(%zu) there are %zu components left.", typeid(ItemType).name(), nodeId, components.size());
        }
    }

public:
    std::shared_ptr<ItemType> Get(const uint64_t id) const
    {
        if (auto result = FindFirst(id)) {
            return result;
        } else {
            throw std::runtime_error("Entity with id = " + std::to_string(id) + " does not exist in this repository.");
        }
    }

    std::vector<std::shared_ptr<ItemType>> GetAll(const uint64_t id) const
    {
        const auto& result{ FindAll(id) };
        if (!result.empty()) {
            return result;
        } else {
            throw std::runtime_error("Entity with id = " + std::to_string(id) + " does not exist in this repository.");
        }
    }

    void Add(const uint64_t id, const std::vector<std::shared_ptr<ItemType>>& components)
    {
        auto addedComponents{ FindAll(id) };
        if (!addedComponents.empty()) {
            for (const auto component : components) {
                if (std::find(addedComponents.cbegin(), addedComponents.cend(), component) != std::end(addedComponents)) {
                    throw std::runtime_error("Entity with id = " + std::to_string(id) + " already exist in this repository.");
                }
            }
        }

        auto& currentComponents{ m_components[id] };
        currentComponents.insert(currentComponents.end(), components.cbegin(), components.cend());
    }

    void Remove(const uint64_t id)
    {
        if (FindFirst(id)) {
            m_components.erase(id);
        } else {
            throw std::runtime_error("Entity with id = " + std::to_string(id) + " does not exist in this repository.");
        }
    }

    bool Contains(const uint64_t id) const
    {
        return !!FindFirst(id);
    }

    std::shared_ptr<ItemType> FindFirst(const uint64_t id) const
    {
        auto iter{ m_components.find(id) };
        if (iter != m_components.cend()) {
            return iter->second.front();
        }
        return {};
    }

    std::vector<std::shared_ptr<ItemType>> FindAll(const uint64_t id) const
    {
        auto iter{ m_components.find(id) };
        if (iter != m_components.cend()) {
            return iter->second;
        }
        return {};
    }

public:
    void operator()(const prev::scene::SceneNodeShutDownEvent& evt)
    {
        if (Contains(evt.id)) {
            Remove(evt.id);
        }
    }

private:
    ComponentRepository() = default;

private:
    friend class prev::common::pattern::Singleton<ComponentRepository<ItemType>>;

private:
    std::map<uint64_t, std::vector<std::shared_ptr<ItemType>>> m_components;

    prev::event::EventHandler<ComponentRepository<ItemType>, prev::scene::SceneNodeShutDownEvent> m_shutDownHandler{ *this };
};
} // namespace prev::scene::component

#endif