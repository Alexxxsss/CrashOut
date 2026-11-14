#pragma once
#include <iostream>

namespace Tga
{
	template <typename T>
	class Vector2;

}
namespace CommonUtilities
{
	template <typename T>
	class Vector2
	{
	public:
		T x; 
		T y; 

		Vector2() : x(0), y(0) {}
		Vector2(const T& aX, const T& aY) : x(aX), y(aY) {}
		Vector2(const Vector2<T>& aVector) = default;
		Vector2<T>& operator=(const Vector2<T>& aVector2) = default;
		~Vector2() = default;
		template<class TargetType>
		Vector2<TargetType> ToType() const
		{
			return Vector2<TargetType>(static_cast<TargetType>(x), static_cast<TargetType>(y));
		}

		Tga::Vector2<T> ToTga() const
		{
			return Tga::Vector2(x, y);
		}

		Vector2<T> operator-() const
		{
			return Vector2<T>(-x, -y); 
		}

		T LengthSqr() const 
		{
			return (x*x) + (y*y);
		}

		T Length() const
		{
			return static_cast<T>(std::sqrt((x*x) + (y*y)));
		}

		Vector2<T> GetNormalized() const
		{
			if (Length() == 0)
			{
				return {0,0};
			}

			return Vector2<T>(x, y) / Length();
		}
		void Normalize()
		{
			if (Length() == 0)
			{
				x = 0;
				y = 0;
				return;
			}
			Vector2<T> normalizedValue = Vector2<T>(x,y) / Length();
			x = normalizedValue.x;
			y = normalizedValue.y;
		}
		T Dot(const Vector2<T>& aVector) const
		{
			return (x * aVector.x) + (y * aVector.y);
		}
		T Cross(const Vector2<T>& aVector) const
		{
			return (x * aVector.y) - (y * aVector.x);
		}
	};
	template <typename T>
	inline Vector2<T> operator+(const Vector2<T>& lhs, const Vector2<T>& rhs)
	{
		return Vector2<T>(lhs.x + rhs.x, lhs.y + rhs.y);
	}
	template <typename T>
	inline Vector2<T> operator-(const Vector2<T>& lhs, const Vector2<T>& rhs)
	{
		return Vector2<T>(lhs.x - rhs.x, lhs.y - rhs.y);
	}
	template <typename T>
	inline Vector2<T> operator*(const Vector2<T>& lhs, const Vector2<T>& rhs)
	{
		return Vector2<T>(lhs.x * rhs.x, lhs.y * rhs.y);
	}
	template <typename T>
	inline Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar)
	{
		return Vector2<T>(aVector.x * aScalar, aVector.y * aScalar);
	}
	template <typename T>
	inline Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector)
	{
		return Vector2<T>(aVector.x * aScalar, aVector.y * aScalar);
	}
	template <typename T>
	inline Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar)
	{
		return Vector2<T>(aVector.x / aScalar, aVector.y / aScalar);
	}
	template <typename T>
	inline Vector2<T>& operator+=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0 = Vector2<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y);
		return aVector0;
	}
	template <typename T>
	inline Vector2<T>& operator-=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0 = Vector2<T>(aVector0.x - aVector1.x, aVector0.y - aVector1.y);
		return aVector0;
	}
	template <typename T>
	inline Vector2<T>& operator*=(Vector2<T>& aVector, const T& aScalar)
	{
		aVector = Vector2<T>(aVector.x * aScalar, aVector.y * aScalar);
		return aVector;
	}
	template <typename T>
	inline Vector2<T>& operator/=(Vector2<T>& aVector, const T& aScalar)
	{
		aVector = Vector2<T>(aVector.x / aScalar, aVector.y / aScalar);
		return aVector;
	}
}


