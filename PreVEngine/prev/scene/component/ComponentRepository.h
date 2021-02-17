#ifndef __COMPONENT_REPOSITORY_H__
#define __COMPONENT_REPOSITORY_H__

#include "../../common/pattern/Singleton.h"
#include "../SceneEvents.h"

#include <prev/event/EventHandler.h>

#include <map>
#include <memory>
#include <vector>

namespace prev::scene::component {
template <typename ItemType>
class ComponentRepository final : public prev::common::pattern::Singleton<ComponentRepository<ItemType> > {
public:
    ~ComponentRepository() = default;

public:
    std::shared_ptr<ItemType> Get(const uint64_t id) const
    {
        if (!Contains(id)) {
            throw std::runtime_error("Entitity with id = " + std::to_string(id) + " does not exist in this repository.");
        }

        return m_components.at(id).front();
    }

    const std::vector<std::shared_ptr<ItemType> >& GetAll(const uint64_t id) const
    {
        if (!Contains(id)) {
            throw std::runtime_error("Entitity with id = " + std::to_string(id) + " does not exist in this repository.");
        }

        return m_components.at(id);
    }

    void Add(const uint64_t id, const std::vector<std::shared_ptr<ItemType> >& components)
    {
        if (Contains(id)) {
            const auto& addedComponents{ m_components.at(id) };
            for (const auto component : components) {
                if (std::find(addedComponents.cbegin(), addedComponents.cend(), component) != std::end(addedComponents)) {
                    throw std::runtime_error("Entitity with id = " + std::to_string(id) + " already exist in this repository.");
                }
            }
        }



        for (const auto component : components) {
            m_components[id].push_back(component);
        }
    }

    void Remove(const uint64_t id)
    {
        if (!Contains(id)) {
            throw std::runtime_error("Entitity with id = " + std::to_string(id) + " does not exist in this repository.");
        }

        m_components.erase(id);
    }

    bool Contains(const uint64_t id) const
    {
        return m_components.find(id) != m_components.cend();
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
    friend class prev::common::pattern::Singleton<ComponentRepository<ItemType> >;

private:
    std::map<uint64_t, std::vector<std::shared_ptr<ItemType> > > m_components;

    prev::event::EventHandler<ComponentRepository<ItemType>, prev::scene::SceneNodeShutDownEvent> m_shutDownHandler{ *this };
};
} // namespace prev::scene::component

#endif