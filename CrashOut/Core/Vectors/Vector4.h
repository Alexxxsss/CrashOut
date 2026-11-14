#pragma once
#include <iostream>

namespace Tga
{
	template <typename T>
	class Vector4;
}
namespace CommonUtilities
{
	template <typename T>
	class Vector4
	{
	public:
		T x; // Note: this variable is explicitly public.
		T y; // Note: this variable is explicitly public.
		T z; // Note: this variable is explicitly public.
		T w;
		// Creates a zero-vector
		Vector4() : x(0), y(0), z(0), w(0) {}
		// Creates a vector (aX, aY, aZ)
		Vector4(const T& aX, const T& aY, const T& aZ, const T& aW) : x(aX), y(aY), z(aZ), w(aW) {}
		// Copy constructor (compiler generated)
		Vector4(const Vector4<T>& aVector) = default;
		// Assignment operator (compiler generated)
		Vector4<T>& operator=(const Vector4<T>& aVector4) = default;
		// Destructor (compiler generated)
		~Vector4() = default;
		// Returns a CommonUtilities::Vector4 copy with another datatype as elements,
		// ex: converts from Vector4<int> to Vector4<float>
		// to explicitly do operations, such as addition, for an explicit type
		template<class TargetType>
		Vector4<TargetType> ToType() const
		{
			return Vector4<TargetType>(static_cast<TargetType>(x), static_cast<TargetType>(y), static_cast<TargetType>(z), static_cast<TargetType>(w));
		}
		// Returns a copy of the vector as a Tga vector, use to interface with TGE
		Tga::Vector4<T> ToTga() const
		{
			return Tga::Vector4(x, y, z, w);
		}
		// Returns a negated copy of the vector
		Vector4<T> operator-() const
		{
			return Vector4<T>(-x, -y, -z, -w);
		}
		// Returns the squared length of the vector, optimization compared to length
		T LengthSqr() const
		{
			return ((x * x) + (y * y) + (z * z) + (w * w));

		}
		// Returns the length of a vector, int vector is not required to work
		T Length() const
		{
			return static_cast<T>(std::sqrt((x * x) + (y * y) + (z * z) + (w*w)));
		}
		// Returns a normalized copy of this vector
		// For int vectors, expected is integer division
		// example (1,2,3)/5 = (0, 0, 0) should be expected
		// Handle normalization of zero-vector by returning the zero vector
		Vector4<T> GetNormalized() const
		{
			if (Length() == 0)
			{
				return { 0,0,0,0 };
			}
			return Vector4<T>(x, y, z, w) / Length();
		}
		// Normalizes the vector
		// For int vectors, expected is integer division
		// example (1,2,3)/5 = (0, 0, 0) should be expected
		// Handle normalization of zero-vector by not modifying the vector
		void Normalize()
		{
			if (Length() == 0)
			{
				x = 0;
				y = 0;
				z = 0;
				w = 0;
				return;
			}
			Vector4<T> normalizedValue = Vector4<T>(x, y, z,w) / Length();
			x = normalizedValue.x;
			y = normalizedValue.y;
			z = normalizedValue.z;
			w = normalizedValue.w;
		}
		// Returns the dot product of this and aVector
		T Dot(const Vector4<T>& aVector) const
		{
			return (x * aVector.x) + (y * aVector.y) + (z * aVector.z) + (w * aVector.w);
		}
		
	};
	// Returns the resulting vector sum from performing addition:
	// lhs + rhs
	template <typename T>
	inline Vector4<T> operator+(const Vector4<T>& lhs, const Vector4<T>& rhs)
	{
		return Vector4<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);

	}
	// Returns the resulting vector sum from performing difference:
	// lhs - rhs
	template <typename T>
	inline Vector4<T> operator-(const Vector4<T>& lhs, const Vector4<T>& rhs)
	{
		return Vector4<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);

	}
	// Returns the resulting vector from performing component wise multiplication:
	// lhs * rhs
	template <typename T>
	inline Vector4<T> operator*(const Vector4<T>& lhs, const Vector4<T>& rhs)
	{
		return Vector4<T>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);

	}
	// Returns the resulting scaled vector from performing scalar multiplication:
	// aVector * aScalar
	template <typename T>
	inline Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar)
	{
		return Vector4<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar, aVector.w * aScalar);
	}
	// Returns the resulting scaled vector from performing scalar multiplication:
	// aScalar * aVector
	template <typename T>
	inline Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector)
	{
		return Vector4<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar, aVector.w * aScalar);
	}
	// Returns the resulting scaled vector from performing scalar division:
	// aVector / aScalar
	template <typename T>
	inline Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar)
	{
		return Vector4<T>(aVector.x / aScalar, aVector.y / aScalar, aVector.z / aScalar, aVector.w / aScalar);
	}
	// Apply and return vector sum from performing addition:
	// lhs += rhs
	// Equivalent to: lhs = lhs+rhs
	template <typename T>
	inline Vector4<T>& operator+=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0 = Vector4<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y, aVector0.z + aVector1.z, aVector0.w + aVector1.w);
		return aVector0;
	}
	// Apply and return vector difference from performing subtraction:
	// lhs -= rhs
	// Equivalent to: lhs = lhs-rhs
	template <typename T>
	inline Vector4<T>& operator-=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0 = Vector4<T>(aVector0.x - aVector1.x, aVector0.y - aVector1.y, aVector0.z - aVector1.z, aVector0.w - aVector1.w);
		return aVector0;

	}
	// Apply and return the scaled vector when performing scalar multiplication:
	// aVector *= aScalar
	// Equivalent to: aVector = aVector * aScalar
	template <typename T>
	inline Vector4<T>& operator*=(Vector4<T>& aVector, const T& aScalar)
	{
		aVector = Vector4<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar, aVector.w * aScalar);
		return aVector;
	}
	// Apply and return the scaled vector when performing scalar division:
	// aVector /= aScalar
	// Equivalent to: aVector = aVector / aScalar
	template <typename T>
	inline Vector4<T>& operator/=(Vector4<T>& aVector, const T& aScalar)
	{
		aVector = Vector4<T>(aVector.x / aScalar, aVector.y / aScalar, aVector.z / aScalar, aVector.w / aScalar);
		return aVector;
	}
}