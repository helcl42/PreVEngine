#ifndef __UTILS_H__
#define __UTILS_H__

#include <vector>
#include <chrono>
#include <iostream>

template <typename Type>
int Clamp(Type val, Type min, Type max)
{
	return (val < min ? min : val > max ? max : val);
}

class FPSService
{
private:
	float m_refreshTimeout = 1.0f;

	std::vector<float> m_deltaTimeSnapshots;

	float m_elpasedTime = 0.0f;

	float m_averageDeltaTime = 0.0f;

public:
	FPSService(float refreshTimeInS = 1.0f)
		: m_refreshTimeout(refreshTimeInS)
	{
	}

	virtual ~FPSService()
	{
	}

public:
	void Update(float deltaTime)
	{
		m_elpasedTime += deltaTime;

		m_deltaTimeSnapshots.push_back(deltaTime);

		if (m_elpasedTime > m_refreshTimeout)
		{
			float deltasSum = 0.0f;
			for (auto & snapshot : m_deltaTimeSnapshots)
			{
				deltasSum += snapshot;
			}
			m_averageDeltaTime = deltasSum / m_deltaTimeSnapshots.size();
			m_elpasedTime = 0.0f;

			m_deltaTimeSnapshots.clear();

			std::cout << "FPS: " << GetAverageFPS() << std::endl;
		}
	}

	float GetAverageDeltaTime() const
	{
		return m_averageDeltaTime;
	}

	float GetAverageFPS() const
	{
		if (m_averageDeltaTime > 0.0f)
		{
			return (1.0f / m_averageDeltaTime);
		}
		return 0.0f;
	}
};

template <class Type>
class IClock
{
public:
	virtual void Reset() = 0;

	virtual void UpdateClock() = 0;

	virtual Type GetDelta() = 0;

public:
	virtual ~IClock()
	{
	}
};

template <class Type>
class Clock : public IClock <Type>
{
private:
	std::chrono::time_point<std::chrono::steady_clock> m_lastFrameTimestamp;

	Type m_frameInterval;

public:
	Clock()
		: m_frameInterval(0.0f)
	{
	}

	virtual ~Clock()
	{
	}

public:
	void Reset();

	void UpdateClock();

	Type GetDelta();
};

template <class Type>
void Clock<Type>::Reset()
{
	m_lastFrameTimestamp = std::chrono::steady_clock::now();
	m_frameInterval = static_cast<Type>(0.0);
}

template <class Type>
void Clock<Type>::UpdateClock()
{
	auto now = std::chrono::steady_clock::now();
	std::chrono::duration<Type, std::milli> fpMS = now - m_lastFrameTimestamp;

	m_frameInterval = static_cast<Type>(fpMS.count() / 1000.0);

	m_lastFrameTimestamp = now;
}

template <class Type>
Type Clock<Type>::GetDelta()
{
	return m_frameInterval;
}

#endif