#ifndef __FLAG_SET_H__
#define __FLAG_SET_H__

#include <bitset>
#include <cassert>
#include <iostream>
#include <string>

namespace PreVEngine {
template <typename T>
class FlagSet {
public:
    FlagSet() = default;

    explicit FlagSet(const T& val)
    {
        flags.set(static_cast<u_type>(val));
    }

    FlagSet& operator&=(const T& val) noexcept
    {
        bool tmp = flags.test(static_cast<u_type>(val));
        flags.reset();
        flags.set(static_cast<u_type>(val), tmp);
        return *this;
    }

    FlagSet& operator&=(const FlagSet& o) noexcept
    {
        flags &= o.flags;
        return *this;
    }

    FlagSet& operator|=(const T& val) noexcept
    {
        flags.set(static_cast<u_type>(val));
        return *this;
    }

    FlagSet& operator|=(const FlagSet& o) noexcept
    {
        flags |= o.flags;
        return *this;
    }

    FlagSet operator&(const T& val) const
    {
        FlagSet ret(*this);
        ret &= val;

        assert(ret.flags.count() <= 1);
        return ret;
    }

    FlagSet operator&(const FlagSet& val) const
    {
        FlagSet ret(*this);
        ret.flags &= val.flags;

        return ret;
    }

    FlagSet operator|(const T& val) const
    {
        FlagSet ret(*this);
        ret |= val;

        assert(ret.flags.count() >= 1);
        return ret;
    }

    FlagSet operator|(const FlagSet& val) const
    {
        FlagSet ret(*this);
        ret.flags |= val.flags;

        return ret;
    }

    FlagSet operator~() const
    {
        FlagSet cp(*this);
        cp.flags.flip();

        return cp;
    }

    explicit operator bool() const
    {
        return flags.any();
    }

    bool operator==(const FlagSet& o) const
    {
        return flags == o.flags;
    }

    std::size_t GetSize() const
    {
        return flags.size();
    }

    std::size_t GetCount() const
    {
        return flags.count();
    }

    FlagSet& Set()
    {
        flags.set();
        return *this;
    }

    FlagSet& Set(const T& val, bool value = true)
    {
        flags.set(static_cast<u_type>(val), value);
        return *this;
    }

    FlagSet& Reset()
    {
        flags.reset();
        return *this;
    }

    FlagSet& Reset(const T& val)
    {
        flags.reset(static_cast<u_type>(val));
        return *this;
    }

    FlagSet& Flip()
    {
        flags.flip();
        return *this;
    }

    FlagSet& Flip(const T& val)
    {
        flags.flip(static_cast<u_type>(val));
        return *this;
    }

    constexpr bool operator[](const T& val) const
    {
        return flags[static_cast<u_type>(val)];
    }

    bool HasAny(const FlagSet<T>& from) const
    {
        auto res = *this & from;
        return res.GetCount() > 0;
    }

    bool HasAll(const FlagSet<T>& from) const
    {
        auto res = *this & from;
        return res.GetCount() >= from.GetCount();
    }

    std::string ToString() const
    {
        return flags.to_string();
    }

    template <typename ReturnType>
    ReturnType ToIntegerumber() const
    {
        return static_cast<ReturnType>(flags.to_ullong());
    }

    // Operator for outputting to std::ostream.
    friend std::ostream& operator<<(std::ostream& stream, const FlagSet& self)
    {
        return stream << self.flags;
    }

private:
    using u_type = std::underlying_type_t<T>;

    // _ is last value sentinel and must be present in enum T.
    std::bitset<static_cast<u_type>(T::_)> flags;
};

template <typename T, typename = void>
struct is_enum_that_contains_sentinel : std::false_type {
};

template <typename T>
struct is_enum_that_contains_sentinel<T, decltype(static_cast<void>(T::_))> : std::is_enum<T> {
};

// Operator that combines two enumeration values into a FlagSet only if the
// enumeration contains the sentinel `_`.
template <typename T>
std::enable_if_t<is_enum_that_contains_sentinel<T>::value, FlagSet<T> > operator|(const T& lhs, const T& rhs)
{
    FlagSet<T> fs;
    fs |= lhs;
    fs |= rhs;
    return fs;
}
} // namespace PreVEngine

#endif