#include "TagSet.h"

namespace PreVEngine {
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

bool TagSet::operator[](const std::string& val) const
{
    return m_tags.find(val) != m_tags.cend();
}
} // namespace PreVEngine