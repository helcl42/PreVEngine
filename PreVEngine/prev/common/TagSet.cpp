#include "TagSet.h"

#include <iostream>
#include <sstream>

namespace prev::common {
TagSet::TagSet(const std::set<std::string>& tags)
    : m_tags(tags)
{
}

TagSet::TagSet(const std::initializer_list<std::string>& tags)
    : m_tags(tags)
{
}

void TagSet::Set(const std::set<std::string>& tags)
{
    m_tags = tags;
}

void TagSet::Set(const std::initializer_list<std::string>& tags)
{
    m_tags = tags;
}

const std::set<std::string>& TagSet::Get() const
{
    return m_tags;
}

void TagSet::Add(const std::string& tag)
{
    m_tags.insert(tag);
}

void TagSet::Remove(const std::string& tag)
{
    m_tags.erase(tag);
}

bool TagSet::Has(const std::string& tag) const
{
    return m_tags.find(tag) != m_tags.cend();
}

bool TagSet::HasAny(const TagSet& tags) const
{
    for (const auto& testedTag : tags.m_tags) {
        if (m_tags.find(testedTag) != m_tags.cend()) {
            return true;
        }
    }
    return false;
}

bool TagSet::HasAll(const TagSet& tags) const
{
    for (const auto& testedTag : tags.m_tags) {
        if (m_tags.find(testedTag) == m_tags.cend()) {
            return false;
        }
    }
    return true;
}

std::string TagSet::ToString() const
{
    std::stringstream ss;
    ss << *this;
    return ss.str();
}

bool TagSet::operator[](const std::string& tag) const
{
    return m_tags.find(tag) != m_tags.cend();
}

TagSet TagSet::operator+(const std::string& tag) const
{
    auto tags{ m_tags };
    tags.insert(tag);
    return { tags };
}

TagSet TagSet::operator+(const TagSet& tagSet) const
{
    auto tags{ m_tags };
    auto newTags{ tagSet.Get() };
    for (const auto& tag : newTags) {
        tags.insert(tag);
    }
    return tags;
}

TagSet& TagSet::operator+=(const std::string& tag)
{
    m_tags.insert(tag);
    return *this;
}

TagSet& TagSet::operator+=(const TagSet& tagSet)
{
    auto newTags{ tagSet.Get() };
    for (const auto& tag : newTags) {
        m_tags.insert(tag);
    }
    return *this;
}

TagSet TagSet::operator-(const std::string& tag) const
{
    auto tags{ m_tags };
    tags.erase(tag);
    return { tags };
}

TagSet TagSet::operator-(const TagSet& tagSet) const
{
    auto tags{ m_tags };
    auto newTags{ tagSet.Get() };
    for (const auto& tag : newTags) {
        tags.erase(tag);
    }
    return tags;
}

TagSet& TagSet::operator-=(const std::string& tag)
{
    m_tags.erase(tag);
    return *this;
}

TagSet& TagSet::operator-=(const TagSet& tagSet)
{
    auto newTags{ tagSet.Get() };
    for (const auto& tag : newTags) {
        m_tags.erase(tag);
    }
    return *this;
}

std::ostream& operator<<(std::ostream& out, const TagSet& tagSet)
{
    out << "[";
    size_t index{ 0 };
    for (const auto& tag : tagSet.m_tags) {
        out << tag;
        if (index < tagSet.m_tags.size() - 1) {
            out << ' ';
        }
        ++index;
    }
    out << "]";
    return out;
}
} // namespace prev::common