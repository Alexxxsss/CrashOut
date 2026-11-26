#pragma once
#include <iostream>

namespace Tga
{
	template <typename T>
	class Vector3;
}
namespace CommonUtilities
{
	template <typename T>
	class Vector3
	{
	public:
		T x; 
		T y; 
		T z; 
		Vector3() : x(0), y(0), z(0){}
		Vector3(const T& aX, const T& aY, const T& aZ) : x(aX), y(aY), z(aZ) {}
		Vector3(const Vector3<T>& aVector) = default;
		Vector3<T>& operator=(const Vector3<T>& aVector3) = default;
		~Vector3() = default;

		template<class TargetType>
		Vector3<TargetType> ToType() const
		{
			return Vector3<TargetType>(static_cast<TargetType>(x), static_cast<TargetType>(y), static_cast<TargetType>(z));
		}
		
		Tga::Vector3<T> ToTga() const
		{
			return Tga::Vector3(x, y, z);
		}

		Vector3<T> operator-() const
		{
			return Vector3<T>(-x, -y,-z);
		}

		T LengthSqr() const
		{
			return ((x*x) + (y * y) + (z * z));
		}

		T Length() const
		{
			return static_cast<T>(std::sqrt((x*x) + (y*y) + (z*z)));
		}

		Vector3<T> GetNormalized() const
		{
			if (Length() == 0)
			{
				return {0,0,0};
			}
			return Vector3<T>(x, y,z) / Length();
		}

		void Normalize()
		{
			if (Length() == 0)
			{
				x = 0;
				y = 0;
				z = 0;
				return;
			}
			Vector3<T> normalizedValue = Vector3<T>(x, y, z) / Length();
			x = normalizedValue.x;
			y = normalizedValue.y;
			z = normalizedValue.z;
		}

		T Dot(const Vector3<T>& aVector) const
		{
			return (x * aVector.x) + (y * aVector.y) + (z * aVector.z);
		}

		Vector3<T> Cross(const Vector3<T>& aVector) const
		{
			return Vector3<T>(((y * aVector.z) - (z * aVector.y)), ((z *aVector.x) - (x * aVector.z)), ((x * aVector.y) - (y * aVector.x)));
		}
	};
	template <typename T>
	inline Vector3<T> operator+(const Vector3<T>& lhs, const Vector3<T>& rhs)
	{
		return Vector3<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);

	}
	template <typename T>
	inline Vector3<T> operator-(const Vector3<T>& lhs, const Vector3<T>& rhs)
	{
		return Vector3<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);

	}
	template <typename T>
	inline Vector3<T> operator*(const Vector3<T>& lhs, const Vector3<T>& rhs)
	{
		return Vector3<T>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);

	}
	template <typename T>
	inline Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar)
	{
		return Vector3<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar);
	}
	template <typename T>
	inline Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector)
	{
		return Vector3<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar);

	}
	template <typename T>
	inline Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar)
	{
		return Vector3<T>(aVector.x / aScalar, aVector.y / aScalar, aVector.z / aScalar);
	}
	template <typename T>
	inline Vector3<T>& operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0 = Vector3<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y, aVector0.z + aVector1.z);
		return aVector0;
	}
	template <typename T>
	inline Vector3<T>& operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0 = Vector3<T>(aVector0.x - aVector1.x, aVector0.y - aVector1.y, aVector0.z - aVector1.z);
		return aVector0;

	}
	template <typename T>
	inline Vector3<T>& operator*=(Vector3<T>& aVector, const T& aScalar)
	{
		aVector = Vector3<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar);
		return aVector;
	}
	template <typename T>
	inline Vector3<T>& operator/=(Vector3<T>& aVector, const T& aScalar)
	{
		aVector = Vector3<T>(aVector.x / aScalar, aVector.y / aScalar, aVector.z / aScalar);
		return aVector;
	}
}