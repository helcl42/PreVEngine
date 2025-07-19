#ifndef __CACHE_H__
#define __CACHE_H__

#include <memory>
#include <optional>
#include <unordered_map>

namespace prev::common {

// TODO: no size limit
// TODO: not thread-safe
template <typename KeyType, typename ValueType>
class Cache final {
public:
    std::optional<ValueType> Find(const KeyType& key) const
    {
        auto iter = m_storage.find(key);
        if (iter == m_storage.cend()) {
            return {};
        }
        return iter->second;
    }

    bool Contains(const KeyType& key) const
    {
        return m_storage.find(key) == m_storage.cend();
    }

    void Add(const KeyType& key, const ValueType& value)
    {
        m_storage[key] = value;
    }

    void Remove(const KeyType& key)
    {
        if (Contains(key)) {
            m_storage.erase(key);
        }
    }

    void Clear()
    {
        m_storage.clear();
    }

private:
    std::unordered_map<KeyType, ValueType> m_storage;
};
} // namespace prev::common

#endif // !__CACHE_H__