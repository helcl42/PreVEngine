#ifndef __OBSERVER_H__
#define __OBSERVER_H__

#include <set>

namespace PreVEngine {
template <typename ObserverType>
class Observer {
public:
    Observer() = default;

    virtual ~Observer() = default;

public:
    bool RegisterListener(ObserverType& listener)
    {
        m_observers.insert(&listener);
        return true;
    }

    bool UnregisterListener(ObserverType& listener)
    {
        auto it = std::find(m_observers.begin(), m_observers.end(), &listener);
        if (it != m_observers.end()) {
            m_observers.erase(it);
            return true;
        }
        return false;
    }

    bool IsListenerRegistered(ObserverType& listener) const
    {
        return m_observers.find(&listener) != m_observers.cend();
    }

    const std::set<ObserverType*>& GetObservers() const
    {
        return m_observers;
    }

protected:
    std::set<ObserverType*> m_observers;
};
} // namespace PreVEngine

#endif