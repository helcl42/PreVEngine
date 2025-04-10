#ifndef _TAG_SET_H__
#define _TAG_SET_H__

#include <set>
#include <string>

namespace prev::common {
class TagSet {
public:
    TagSet() = default;

    TagSet(const std::set<std::string>& tags);

    TagSet(const std::initializer_list<std::string>& tags);

public:
    void Set(const std::set<std::string>& tags);

    void Set(const std::initializer_list<std::string>& tags);

    const std::set<std::string>& Get() const;

    void Add(const std::string& tag);

    void Remove(const std::string& tag);

    bool Has(const std::string& tag) const;

    bool HasAny(const TagSet& tags) const;

    bool HasAll(const TagSet& tags) const;

    bool operator[](const std::string& tag) const;

    TagSet operator+(const std::string& tag) const;

    TagSet operator+(const TagSet& tagSet) const;

    TagSet& operator+=(const std::string& tag);

    TagSet& operator+=(const TagSet& tagSet);

    TagSet operator-(const std::string& tag) const;

    TagSet operator-(const TagSet& tagSet) const;

    TagSet& operator-=(const std::string& tag);

    TagSet& operator-=(const TagSet& tagSet);

    friend std::ostream& operator<<(std::ostream& out, const TagSet& tagSet);

private:
    std::set<std::string> m_tags;
};

} // namespace prev::common

#endif