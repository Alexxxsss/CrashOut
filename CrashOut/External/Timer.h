#pragma once
#include <iostream>
#include <chrono>
#include <cstdint>
namespace CommonUtilities
{
	class Timer
	{
	public:
		Timer(float aFixedTickTime = 1.f / 60.f);
		Timer(const Timer& aTimer) = delete;
		Timer& operator=(const Timer& aTimer) = delete;
		void Update();
		float GetDeltaTime() const;
		double GetTotalTime() const;
		int GetFixedRateTickDeltaCount() const;
		uint64_t GetFixedRateTickCount() const;
	private:
		using Clock = std::chrono::high_resolution_clock;
		using TimePoint = std::chrono::time_point<Clock>;

		TimePoint myStartTime;
		TimePoint myPreviousTime;
		TimePoint myCurrentTime;

		float myDeltaTime;
		double myTotalTime;
		float myFixedTickTime;

		uint64_t myCurrentFixedTicks;
		uint64_t myPreviousFixedTicks;

	};
}
