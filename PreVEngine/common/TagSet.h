#ifndef _TAG_SET_H__
#define _TAG_SET_H__

#include <set>
#include <string>

namespace PreVEngine {
class TagSet {
public:
    TagSet() = default;

    TagSet(const std::set<std::string>& tags);

    TagSet(const std::initializer_list<std::string>& tags);

    void Set(const std::set<std::string>& tags);

    void Set(const std::initializer_list<std::string>& tags);

    void Add(const std::string& tag);

    void Remove(const std::string& tag);

    bool Has(const std::string& tag) const;

    bool HasAny(const TagSet& tags) const;

    bool HasAll(const TagSet& tags) const;

    bool operator[](const std::string& val) const;

private:
    std::set<std::string> m_tags;
};

} // namespace PreVEngine

#endif