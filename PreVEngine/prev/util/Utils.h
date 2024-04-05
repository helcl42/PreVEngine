#ifndef __UTILS_H__
#define __UTILS_H__

#include "../common/Common.h"
#include "../common/pattern/Singleton.h"

#include <atomic>
#include <chrono>
#include <cmath>
#include <random>
#include <vector>

namespace prev::util {

template <typename T, ptrdiff_t N>
inline ptrdiff_t ArraySize(T (&)[N])
{
    return N;
}

namespace file {
    bool CreateDirectoryByPath(const std::string& path);

    std::string GetDirectoryPath(const std::string& filePath);

    std::string GetFileName(const std::string& filePath);
    
    bool Exists(const std::string& filePath);

    std::string ReadTextFile(const std::string& filePath);

    std::vector<char> ReadBinaryFile(const std::string& filePath);
} // namespace file

namespace string {
    std::vector<std::string> Split(const std::string& s, const char delim);

    std::vector<std::wstring> Split(const std::wstring& s, const wchar_t delim);

    std::vector<std::string> Split(const std::string& s, const std::string& t);

    std::vector<std::wstring> Split(const std::wstring& s, const std::wstring& t);

    std::string Replace(const std::string& subject, const std::string& search, const std::string& replace);

    std::wstring Replace(const std::wstring& subject, const std::wstring& search, const std::wstring& replace);

    std::string GetAsString(const glm::vec2& v, const uint32_t precission);

    std::string GetAsString(const glm::vec3& v, const uint32_t precission);

    std::string GetAsString(const glm::vec4& v, const uint32_t precission);

    std::string GetAsString(const glm::quat& q, const uint32_t precission);
} // namespace string

class FPSCounter final {
public:
    FPSCounter(float refreshTimeInS = 2.0f)
        : m_refreshTimeoutInS(refreshTimeInS)
    {
        Reset();
    }

    ~FPSCounter() = default;

public:
    bool Tick()
    {
        const auto Now{ std::chrono::high_resolution_clock::now() };
        if (m_lastTickTimestamp != std::chrono::high_resolution_clock::time_point::min()) {
            m_deltaSum += std::chrono::duration<float>(Now - m_lastTickTimestamp).count();
            ++m_deltaCount;
        }
        m_lastTickTimestamp = Now;

        if (m_deltaSum > m_refreshTimeoutInS) {
            m_averageDeltaTime = m_deltaSum / static_cast<float>(m_deltaCount);
            m_deltaSum = 0.0f;
            m_deltaCount = 0;
            return true;
        }
        return false;
    }

    void Reset()
    {
        m_lastTickTimestamp = std::chrono::high_resolution_clock::time_point::min();
    }

    float GetAverageDeltaTime() const
    {
        return m_averageDeltaTime;
    }

    float GetAverageFPS() const
    {
        if (m_averageDeltaTime > 0.0f) {
            return (1.0f / m_averageDeltaTime);
        }
        return 0.0f;
    }

private:
    float m_refreshTimeoutInS{ 1.0f };

    float m_deltaSum{ 0.0f };

    uint32_t m_deltaCount{ 0 };

    float m_averageDeltaTime{ 0.0f };

    std::chrono::high_resolution_clock::time_point m_lastTickTimestamp{};
};

template <typename Type, size_t Size>
class CircularQueue {
public:
    bool IsEmpty() const
    {
        return m_head == m_tail;
    }

    void Push(const Type& item)
    {
        m_head = ++m_head % Size;
        m_buffer[m_head] = item;
    }

    Type& Pop()
    {
        m_tail = ++m_tail % Size;
        return m_buffer[m_tail];
    }

    void Clear()
    {
        for (int i = 0; i < Size; ++i) {
            m_buffer[i] = {};
        }
        m_head = 0;
        m_tail = 0;
    }

private:
    uint32_t m_head{};

    uint32_t m_tail{};

    Type m_buffer[Size] = {};
};

template <class Type>
class Clock final {
public:
    Clock()
    {
        Reset();
    }

    ~Clock() = default;

public:
    void Reset()
    {
        m_lastFrameTimestamp = std::chrono::steady_clock::now();
        m_frameInterval = static_cast<Type>(0.0);
    }

    void UpdateClock()
    {
        const auto Now{ std::chrono::steady_clock::now() };
        m_frameInterval = std::chrono::duration<Type>(Now - m_lastFrameTimestamp).count();
        m_lastFrameTimestamp = Now;
    }

    Type GetDelta() const
    {
        return m_frameInterval;
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> m_lastFrameTimestamp;

    Type m_frameInterval;
};

class IDGenerator final : public prev::common::pattern::Singleton<IDGenerator> {
private:
    friend class prev::common::pattern::Singleton<IDGenerator>;

private:
    IDGenerator() = default;

public:
    ~IDGenerator() = default;

public:
    uint64_t GenrateNewId()
    {
        return ++m_id;
    }

    void Reset()
    {
        m_id = 0;
    }

private:
    std::atomic_uint64_t m_id{ 0 };
};

class RandomNumberGenerator {
public:
    RandomNumberGenerator()
        : m_gen(std::random_device()())
    {
    }

    RandomNumberGenerator(unsigned long seed)
        : m_gen(seed)
    {
    }

    std::default_random_engine& GetRandomEngine()
    {
        return m_gen;
    }

private:
    std::default_random_engine m_gen;
};

class UUIDGenerator {
public:
    static std::string GenerateNew()
    {
        std::string uuid = std::string(36, '-');

        RandomNumberGenerator rng{};
        for (uint32_t i = 0; i < 36; ++i) {
            if (i != 8 && i != 13 && i != 18 && i != 23) {
                uuid[i] = GetRandomSymbol(rng);
            }
        }

        return uuid;
    }

    static std::string GenerateEmpty()
    {
        std::string uuid = std::string(36, '0');

        uuid[8] = '-';
        uuid[13] = '-';
        uuid[18] = '-';
        uuid[23] = '-';

        return uuid;
    }

private:
    static char GetRandomSymbol(RandomNumberGenerator& rng)
    {
        static const std::string validSymbols{ "0123456789abcdef" };

        std::uniform_int_distribution<> dist(0, static_cast<int>(validSymbols.size() - 1));
        const auto index{ dist(rng.GetRandomEngine()) };
        return validSymbols[index];
    }
};

} // namespace prev::util

#endif
