#ifndef __SINGLETON_H__
#define __SINGLETON_H__

namespace prev::common::pattern {
template <typename ChildType>
class Singleton {
private:
    friend ChildType;

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
        static ChildType instance;
        return instance;
    }
};
} // namespace prev::common::pattern

#endif