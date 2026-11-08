#pragma once
#include <cassert>
namespace CommonUtilities
{

	template<typename T>
	T Min(T aFirstValue, T aSecondValue)
	{
		return aFirstValue < aSecondValue ? aFirstValue : aSecondValue;
	}

	template<typename T>
	T Max(T aFirstValue, T aSecondValue)
	{
		return aFirstValue > aSecondValue ? aFirstValue : aSecondValue;
	}

	template<typename T>
	T Abs(T aValue)
	{
		return aValue < 0 ? aValue *-1 : aValue;
	}

	template<typename T>
	T Clamp(T aValue, T aMinValue, T aMaxValue)
	{
		assert(aMinValue <= aMaxValue && "aMinValue får inte vara större än aMaxValue!");		
		if (aValue > aMaxValue) 
		{
			return aMaxValue;
		}
		if (aValue < aMinValue) 
		{
			return aMinValue;
		}
		return aValue;
	}

	template<typename T>
	T Lerp(T aFirstValue, T aSecondValue, float aLerpPercentage)
	{
		return aFirstValue + aLerpPercentage * (aSecondValue - aFirstValue);
	}

	template<typename T>
	void Swap(T &aFirstValue, T &aSecondValue)
	{
		T temporaryData = aFirstValue;
		aFirstValue = aSecondValue;
		aSecondValue = temporaryData;
	}

}