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
private:
    float m_refreshTimeoutInS{ 1.0f };

    std::vector<float> m_deltaTimeSnapshots;

    float m_elpasedTimeInS{ 0.0f };

    float m_averageDeltaTime{ 0.0f };

    std::chrono::high_resolution_clock::time_point m_lastTickTimestamp{ std::chrono::high_resolution_clock::time_point::min() };

public:
    FPSCounter(float refreshTimeInS = 2.0f)
        : m_refreshTimeoutInS(refreshTimeInS)
    {
    }

    ~FPSCounter() = default;

public:
    bool Tick()
    {
        const auto NOW = std::chrono::high_resolution_clock::now();
        if (m_lastTickTimestamp == std::chrono::high_resolution_clock::time_point::min()) {
            m_lastTickTimestamp = NOW;
        }

        float elapsedInS = std::chrono::duration<float>(NOW - m_lastTickTimestamp).count();
        m_deltaTimeSnapshots.push_back(elapsedInS);
        m_elpasedTimeInS += elapsedInS;

        m_lastTickTimestamp = NOW;

        if (m_elpasedTimeInS > m_refreshTimeoutInS) {
            float deltasSum = 0.0f;
            for (const auto& snapshot : m_deltaTimeSnapshots) {
                deltasSum += snapshot;
            }
            m_averageDeltaTime = deltasSum / m_deltaTimeSnapshots.size();

            m_elpasedTimeInS = 0.0f;
            m_deltaTimeSnapshots.clear();
            return true;
        }
        return false;
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
};

template <class Type>
class Clock final {
private:
    std::chrono::time_point<std::chrono::steady_clock> m_lastFrameTimestamp;

    Type m_frameInterval;

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
        const auto now{ std::chrono::steady_clock::now() };
        m_frameInterval = std::chrono::duration<Type>(now - m_lastFrameTimestamp).count();
        m_lastFrameTimestamp = now;
    }

    Type GetDelta() const
    {
        return m_frameInterval;
    }
};

class IDGenerator final : public prev::common::pattern::Singleton<IDGenerator> {
private:
    friend class prev::common::pattern::Singleton<IDGenerator>;

private:
    std::atomic_uint64_t m_id{ 0 };

private:
    IDGenerator() = default;

public:
    ~IDGenerator() = default;

public:
    uint64_t GenrateNewId()
    {
        return ++m_id;
    }
};

class UUIDGenerator {
public:
    static std::string GenerateNew()
    {
        std::string uuid = std::string(36, ' ');

        uuid[8] = '-';
        uuid[13] = '-';
        uuid[18] = '-';
        uuid[23] = '-';

        for (uint32_t i = 0; i < 36; i++) {
            if (i != 8 && i != 13 && i != 18 && i != 23) {
                uuid[i] = GetRandomSymbol();
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
    static char GetRandomSymbol()
    {
        static const std::string validSymbols = "0123456789abcdef";

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, static_cast<int>(validSymbols.size() - 1));
        const auto index = dis(gen);
        return validSymbols[index];
    }
};

} // namespace prev::util

#endif