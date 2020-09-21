#ifndef __NULLABLE_H__
#define __NULLABLE_H__

namespace prev::common::pattern {
template <typename ObjectType>
class Nullable {
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

private:
    bool m_null;

    ObjectType m_value;
};
} // namespace prev::common::pattern
#endif