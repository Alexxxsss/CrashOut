#include "Timer.h"
#include <iostream>
#include <chrono>



namespace CommonUtilities
{
	Timer::Timer(float aFixedTickTime)
	{
		myStartTime = Clock::now();
		myPreviousTime = myStartTime;
		myCurrentTime = myStartTime;

		// Initiera variabler
		myDeltaTime = 0.0f;
		myTotalTime = 0.0;
		myFixedTickTime = aFixedTickTime;

		myCurrentFixedTicks = 0;
		myPreviousFixedTicks = 0;
	}

	void Timer::Update()
	{
		myPreviousTime = myCurrentTime;
		myPreviousFixedTicks = myCurrentFixedTicks;

		myCurrentTime = Clock::now();

		//Calculate Delta Time
		std::chrono::duration<float> deltaDuration = myCurrentTime - myPreviousTime;
		myDeltaTime = deltaDuration.count();

		//Calculate Total Time
		std::chrono::duration<double> totalDuration = myCurrentTime - myStartTime;
		myTotalTime = totalDuration.count();

		myCurrentFixedTicks = GetFixedRateTickCount();

	}
	float Timer::GetDeltaTime() const
	{
		return myDeltaTime;
	}
	double Timer::GetTotalTime() const
	{
		return myTotalTime;
	}
	int Timer::GetFixedRateTickDeltaCount() const
	{
		return static_cast<int>(myCurrentFixedTicks - myPreviousFixedTicks);
	}
	uint64_t Timer::GetFixedRateTickCount() const
	{
		return static_cast<uint64_t>(myTotalTime / myFixedTickTime);
	}
}