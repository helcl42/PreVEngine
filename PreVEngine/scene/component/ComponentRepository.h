#ifndef __COMPONENT_REPOSITORY_H__
#define __COMPONENT_REPOSITORY_H__

#include "../../common/pattern/Singleton.h"
#include "../SceneEvents.h"

#include <map>
#include <memory>

namespace prev {
template <typename ItemType>
class ComponentRepository final : public prev::common::pattern::Singleton<ComponentRepository<ItemType> > {
public:
    ~ComponentRepository() = default;

public:
    std::shared_ptr<ItemType> Get(const uint64_t id) const
    {
        if (!Contains(id)) {
            throw std::runtime_error("Entitity sith id = " + std::to_string(id) + " does not exist in this repository.");
        }

        return m_components.at(id);
    }

    void Add(const uint64_t id, const std::shared_ptr<ItemType>& component)
    {
        if (Contains(id)) {
            throw std::runtime_error("Entitity sith id = " + std::to_string(id) + " already exist in this repository.");
        }

        m_components[id] = component;
    }

    void Remove(const uint64_t id)
    {
        if (!Contains(id)) {
            throw std::runtime_error("Entitity sith id = " + std::to_string(id) + " does not exist in this repository.");
        }

        m_components.erase(id);
    }

    bool Contains(const uint64_t id) const
    {
        return m_components.find(id) != m_components.cend();
    }

    void operator()(const SceneNodeShutDownEvent& evt)
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
    std::map<uint64_t, std::shared_ptr<ItemType> > m_components;

    EventHandler<ComponentRepository<ItemType>, SceneNodeShutDownEvent> m_shutDownHandler{ *this };
};
} // namespace prev

#endif