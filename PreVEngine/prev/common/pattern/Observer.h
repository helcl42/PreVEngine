#ifndef __OBSERVER_H__
#define __OBSERVER_H__

#include <algorithm>
#include <set>

namespace prev::common::pattern {
template <typename ObserverType>
class Observer {
public:
    Observer() = default;

    virtual ~Observer() = default;

public:
    bool Register(ObserverType& o)
    {
        m_observers.insert(&o);
        return true;
    }

    bool Unregister(ObserverType& o)
    {
        auto it = std::find(m_observers.begin(), m_observers.end(), &o);
        if (it != m_observers.end()) {
            m_observers.erase(it);
            return true;
        }
        return false;
    }

    bool IsLRegistered(ObserverType& o) const
    {
        return m_observers.find(&o) != m_observers.cend();
    }

    const std::set<ObserverType*>& GetObservers() const
    {
        return m_observers;
    }

protected:
    std::set<ObserverType*> m_observers;
};
} // namespace prev::common::pattern

#endif