#ifndef __UTILS_H__
#define __UTILS_H__

#include "../common/Common.h"
#include "../common/pattern/Singleton.h"
#include "../core/instance/Validation.h"

#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

namespace prev::util {
// Global functions !!!
template <typename Type, ptrdiff_t n>
ptrdiff_t ArraySize(Type (&)[n])
{
    return n;
}

class File {
public:
    static bool Exists(const std::string& filePath)
    {
        return std::filesystem::exists(filePath);
    }

    static bool CreateDirectoryByPath(const std::string& path)
    {
        return std::filesystem::create_directories(path);
    }

    static std::string GetDirectoryPath(const std::string& filePath)
    {
        std::filesystem::path p(filePath);
        std::filesystem::path parent = p.parent_path();
        return parent.string();
    }
};

class StringUtils {
public:
    static std::vector<std::string> Split(const std::string& s, const char delim)
    {
        std::vector<std::string> elems;
        std::istringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }

    static std::vector<std::wstring> Split(const std::wstring& s, const wchar_t delim)
    {
        std::vector<std::wstring> elems;
        std::wstringstream ss(s);
        std::wstring item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }

    static std::vector<std::string> Split(const std::string& s, const std::string& t)
    {
        std::string copy{ s };
        std::vector<std::string> res;
        while (true) {
            size_t pos = copy.find(t);
            if (pos == std::string::npos) {
                res.push_back(copy);
                break;
            }
            res.push_back(copy.substr(0, pos));
            copy = copy.substr(pos + 1, copy.size() - pos - 1);
        }
        return res;
    }

    static std::vector<std::wstring> Split(const std::wstring& s, const std::wstring& t)
    {
        std::wstring copy{ s };
        std::vector<std::wstring> res;
        while (true) {
            size_t pos = copy.find(t);
            if (pos == std::string::npos) {
                res.push_back(copy);
                break;
            }
            res.push_back(copy.substr(0, pos));
            copy = copy.substr(pos + 1, copy.size() - pos - 1);
        }
        return res;
    }

    static std::string Replace(const std::string& subject, const std::string& search, const std::string& replace)
    {
        std::string copy{ subject };
        size_t pos{ 0 };
        while ((pos = copy.find(search, pos)) != std::string::npos) {
            copy.replace(pos, search.length(), replace);
            pos += replace.length();
        }
        return copy;
    }

    static std::wstring Replace(const std::wstring& subject, const std::wstring& search, const std::wstring& replace)
    {
        std::wstring copy{ subject };
        size_t pos{ 0 };
        while ((pos = copy.find(search, pos)) != std::wstring::npos) {
            copy.replace(pos, search.length(), replace);
            pos += replace.length();
        }
        return copy;
    }

    static std::string GetAsString(const glm::vec2& v, const uint32_t precission)
    {
        std::stringstream ss;
        ss << std::setprecision(precission) << "(" << v.x << ", " << v.y << ")";
        return ss.str();
    }

    static std::string GetAsString(const glm::vec3& v, const uint32_t precission)
    {
        std::stringstream ss;
        ss << std::setprecision(precission) << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return ss.str();
    }

    static std::string GetAsString(const glm::vec4& v, const uint32_t precission)
    {
        std::stringstream ss;
        ss << std::setprecision(precission) << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
        return ss.str();
    }

    static std::string GetAsString(const glm::quat& q, const uint32_t precission)
    {
        std::stringstream ss;
        ss << std::setprecision(precission) << "(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")";
        return ss.str();
    }
};

class FPSService final {
private:
    float m_refreshTimeout = 1.0f;

    bool m_printInfo{ true };

    std::vector<float> m_deltaTimeSnapshots;

    float m_elpasedTime = 0.0f;

    float m_averageDeltaTime = 0.0f;

    mutable std::mutex m_lock;

public:
    FPSService(float refreshTimeInS = 1.0f, bool printInfo = true)
        : m_refreshTimeout(refreshTimeInS)
        , m_printInfo(printInfo)
    {
    }

    ~FPSService() = default;

public:
    bool Update(float deltaTime)
    {
        std::lock_guard<std::mutex> lock(m_lock);

        m_elpasedTime += deltaTime;

        m_deltaTimeSnapshots.push_back(deltaTime);

        if (m_elpasedTime > m_refreshTimeout) {
            float deltasSum = 0.0f;
            for (auto& snapshot : m_deltaTimeSnapshots) {
                deltasSum += snapshot;
            }
            m_averageDeltaTime = deltasSum / static_cast<float>(m_deltaTimeSnapshots.size());
            m_elpasedTime = 0.0f;

            m_deltaTimeSnapshots.clear();

            if (m_printInfo) {
                LOGI("FPS %f\n", (1.0f / m_averageDeltaTime));
            }
            return true;
        }
        return false;
    }

    float GetAverageDeltaTime() const
    {
        std::lock_guard<std::mutex> lock(m_lock);

        return m_averageDeltaTime;
    }

    float GetAverageFPS() const
    {
        std::lock_guard<std::mutex> lock(m_lock);

        if (m_averageDeltaTime > 0.0f) {
            return (1.0f / m_averageDeltaTime);
        }
        return 0.0f;
    }
};

class FPSCounter final {
private:
    float m_refreshTimeoutInS{ 1.0f };

    bool m_printInfo{ true };

    std::vector<float> m_deltaTimeSnapshots;

    float m_elpasedTimeInS{ 0.0f };

    float m_averageDeltaTime{ 0.0f };

    std::chrono::high_resolution_clock::time_point m_lastTickTimestamp{ std::chrono::high_resolution_clock::time_point::min() };

    mutable std::mutex m_lock;

public:
    FPSCounter(float refreshTimeInS = 2.0f, bool printInfo = true)
        : m_refreshTimeoutInS(refreshTimeInS)
        , m_printInfo(printInfo)
    {
    }

    ~FPSCounter() = default;

public:
    bool Tick()
    {
        std::lock_guard<std::mutex> lock(m_lock);

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

            if (m_printInfo) {
                LOGI("FPS %f\n", (1.0f / m_averageDeltaTime));
            }

            m_elpasedTimeInS = 0.0f;
            m_deltaTimeSnapshots.clear();
            return true;
        }
        return false;
    }

    float GetAverageDeltaTime() const
    {
        std::lock_guard<std::mutex> lock(m_lock);

        return m_averageDeltaTime;
    }

    float GetAverageFPS() const
    {
        std::lock_guard<std::mutex> lock(m_lock);

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
        : m_frameInterval(0.0f)
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
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<Type, std::milli> fpMS = now - m_lastFrameTimestamp;

        m_frameInterval = static_cast<Type>(fpMS.count() / 1000.0);

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
    uint64_t m_id = 0;

    std::mutex m_mutex;

private:
    IDGenerator() = default;

public:
    ~IDGenerator() = default;

public:
    uint64_t GenrateNewId()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        m_id++;

        return m_id;
    }
};

class UUIDGenerator {
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
};

} // namespace prev::util

#endif