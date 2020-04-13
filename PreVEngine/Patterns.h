#ifndef __PATTERNS_H__
#define __PATTERNS_H__

#include <set>

namespace PreVEngine {
////////////////////////////////////////////////////////////
// Singleton
////////////////////////////////////////////////////////////
template <typename ChildType>
class Singleton {
private:
    friend ChildType;

private:
    static ChildType s_instance;

private:
    Singleton(const Singleton& other) = delete;

    Singleton(Singleton&& other) = delete;

    Singleton& operator=(const Singleton& other) = delete;

    Singleton& operator=(Singleton&& other) = delete;

private:
    Singleton() = default;

public:
    virtual ~Singleton() = default;

public:
    static ChildType& Instance()
    {
        return s_instance;
    }
};

template <typename ChildType>
ChildType Singleton<ChildType>::s_instance;
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Observer
////////////////////////////////////////////////////////////
template <typename ObserverType>
class Observer {
protected:
    std::set<ObserverType*> m_observers;

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
};
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Nullable
////////////////////////////////////////////////////////////
template <typename ObjectType>
class Nullable {
private:
    bool m_null;

    ObjectType m_value;

public:
    Nullable()
        : m_null(true)
    {
    }

    Nullable(const ObjectType& value)
        : m_null(false)
        , m_value(value)
    {
    }

    virtual ~Nullable() = default;

public:
    bool IsNull() const
    {
        return m_null;
    }

    const ObjectType& GetValue() const
    {
        return m_value;
    }
};
////////////////////////////////////////////////////////////

} // namespace PreVEngine

#endif