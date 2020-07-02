/*****************************************************************\
           __
          / /
		 / /                     __  __
		/ /______    _______    / / / / ________   __       __
	   / ______  \  /_____  \  / / / / / _____  | / /      / /
	  / /      | / _______| / / / / / / /____/ / / /      / /
	 / /      / / / _____  / / / / / / _______/ / /      / /
	/ /      / / / /____/ / / / / / / |______  / |______/ /
   /_/      /_/ |________/ / / / /  \_______/  \_______  /
                          /_/ /_/                     / /
			                                         / /
		       High Level Game Framework            /_/

  ---------------------------------------------------------------

  Copyright (c) 2007-2011 - Rodrigo Braz Monteiro.
  This file is subject to the terms of halley_license.txt.

\*****************************************************************/


#pragma once

#include <cmath>
#include <iostream>
#include "angle.h"
#include <halley/utils/utils.h>
#include "halley/text/string_converter.h"
#include <gsl/gsl>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

namespace Halley {
	//////////////////////////////
	// Vector2D class declaration
	template <typename T=float, class U=Angle<float>>
	class Vector2D {
	private:
		constexpr T mod(T a, T b) const { return a % b; }

	public:
		T x,y;

		// Constructors
		constexpr Vector2D () : x(0), y(0) {}
		
		constexpr Vector2D (T x, T y) : x(x), y(y) {}
		
		template <typename V>
		constexpr explicit Vector2D (Vector2D<V> vec) : x(T(vec.x)), y(T(vec.y)) {}

		Vector2D (T length, U angle)
		{
			float s, c;
			angle.sincos(s, c);
			x = s*length;
			y = c*length;
		}

		// Getter
		constexpr inline T& operator[](size_t n)
		{
			Expects(n <= 1);
			return (&x)[n];
		}
		constexpr inline T operator[](size_t n) const
		{
			Expects(n <= 1);
			return (&x)[n];
		}

		// Assignment and comparison
		constexpr inline Vector2D& operator = (Vector2D param) { x = param.x; y = param.y; return *this; }
		constexpr inline Vector2D& operator = (T param) { x = param; y = param; return *this; }
		constexpr inline bool operator == (Vector2D param) const { return x == param.x && y == param.y; }
		constexpr inline bool operator != (Vector2D param) const { return x != param.x || y != param.y; }
		constexpr inline bool operator < (Vector2D param) const { return y != param.y ? y < param.y : x < param.x; }

		// Basic algebra
		constexpr inline Vector2D operator + (Vector2D param) const { return Vector2D(x + param.x,y + param.y); }
		constexpr inline Vector2D operator - (Vector2D param) const { return Vector2D(x - param.x,y - param.y); }
		constexpr inline Vector2D operator * (Vector2D param) const { return Vector2D(x * param.x,y * param.y); }
		constexpr inline Vector2D operator / (Vector2D param) const { return Vector2D(x / param.x,y / param.y); }
		constexpr inline Vector2D operator % (Vector2D param) const { return Vector2D(mod(x, param.x), mod(y, param.y)); }

		constexpr inline Vector2D modulo(Vector2D param) const { return Vector2D(Halley::modulo<T>(x, param.x), Halley::modulo<T>(y, param.y)); }
		constexpr inline Vector2D floorDiv(Vector2D param) const { return Vector2D(Halley::floorDiv(x, param.x), Halley::floorDiv(y, param.y)); }

		constexpr inline Vector2D operator - () const { return Vector2D(-x,-y); }

		template <typename V>
		constexpr inline Vector2D operator * (V param) const { return Vector2D(T(x * param), T(y * param)); }

		template <typename V>
		constexpr inline Vector2D operator / (V param) const { return Vector2D(T(x / param), T(y / param)); }

		// In-place operations
		constexpr inline Vector2D& operator += (Vector2D param) { x += param.x; y += param.y; return *this; }
		constexpr inline Vector2D& operator -= (Vector2D param) { x -= param.x; y -= param.y; return *this; }
		constexpr inline Vector2D& operator *= (Vector2D param) { x *= param.x; y *= param.y; return *this; }
		constexpr inline Vector2D& operator /= (Vector2D param) { x /= param.x; y /= param.y; return *this; }
		constexpr inline Vector2D& operator %= (Vector2D param) { x %= param.x; y %= param.y; return *this; }
		constexpr inline Vector2D& operator *= (const T param) { x *= param; y *= param; return *this; }
		constexpr inline Vector2D& operator /= (const T param) { x /= param; y /= param; return *this; }

		// Get the normalized vector (unit vector)
		constexpr inline Vector2D unit () const
		{
			float len = length();
			if (len != 0) {
				return (*this) / len;
			} else {
				return Vector2D(0, 0);
			}
		}
		constexpr inline Vector2D normalized() const
		{
			return unit();
		}
		constexpr inline void normalize()
		{
			*this = unit();
		}

		// Get the orthogonal vector
		constexpr inline Vector2D orthoLeft () const { return Vector2D(-y, x); }
		constexpr inline Vector2D orthoRight () const { return Vector2D(y, -x); }

		// Cross product (the Z component of it)
		constexpr inline T cross (Vector2D param) const { return x * param.y - y * param.x; }

		// Dot product
		constexpr inline T dot (Vector2D param) const { return (x * param.x) + (y * param.y); }

		// Length
		constexpr inline T length () const { return static_cast<T>(std::sqrt(squaredLength())); }
		constexpr inline T len () const { return length(); }
		constexpr inline T manhattanLength() const { return std::abs(x) + std::abs(y); }

		// Squared length, often useful and much faster
		constexpr inline T squaredLength () const { return x*x+y*y; }

		// Projection on another vector
		inline Vector2D projection (Vector2D param) const
		{
			Vector2D unit = param.unit();
			return this->dot(unit) * unit;
		}
		inline T projectionLength (Vector2D param) const
		{
			return this->dot(param.unit());
		}

		// Rounding
		constexpr inline Vector2D floor() const { return Vector2D(std::floor(x), std::floor(y)); }
		constexpr inline Vector2D ceil() const { return Vector2D(std::ceil(x), std::ceil(y)); }
		constexpr inline Vector2D round() const { return Vector2D(std::round(x), std::round(y)); }

		// Gets the angle that this vector is pointing to
		inline U angle () const
		{
			U angle;
			angle.setRadians(atan2(y, x));
			return angle;
		}

		// Rotates vector by an angle
		inline Vector2D rotate (U angle) const
		{
			T cos, sin;
			angle.sincos(sin, cos);
			return Vector2D(x*cos - y*sin, x*sin + y*cos);
		}

		// Rotates vector by sine and cosine
		inline Vector2D rotate (T sine, T cosine) const
		{
			return Vector2D(x*cosine - y*sine, x*sine + y*cosine);
		}

		// Removes the length of the vector along the given axis
		constexpr inline Vector2D neutralize (Vector2D param) const
		{
			return *this - dot(param)*param;
		}

		String toString() const
		{
			return String("(") + x + ", " + y + ")";
		}

		constexpr Vector2D abs() const
		{
			return Vector2D(std::abs(x), std::abs(y));
		}

		constexpr inline static Vector2D<T,U> min(Vector2D<T,U> a, Vector2D<T,U> b)
		{
			return Vector2D<T,U>(std::min(a.x, b.x), std::min(a.y, b.y));
		}

		constexpr inline static Vector2D<T,U> max(Vector2D<T,U> a, Vector2D<T,U> b)
		{
			return Vector2D<T,U>(std::max(a.x, b.x), std::max(a.y, b.y));
		}
	};


	////////////////////
	// Global operators
	template <typename T,class U,typename V>
	inline Vector2D<T,U> operator * (V f, const Vector2D<T,U> &v)
	{
		return Vector2D<T,U>(T(v.x * f),T(v.y * f));
	}

	template <typename T,class U>
	std::ostream& operator<< (std::ostream& ostream, const Vector2D<T, U>& v)
	{
		ostream << "(" << v.x << "," << v.y << ")" ; return ostream;
	}
	
	////////////
	// Typedefs
	typedef Vector2D<double,Angle<double> > Vector2d;
	typedef Vector2D<> Vector2f;
	typedef Vector2D<int> Vector2i;
	typedef Vector2D<short> Vector2s;
	typedef Vector2D<char> Vector2c;
	typedef Vector2f Position;
	typedef Vector2f Size;
}

namespace std {
	template<>
	struct hash<Halley::Vector2i>
	{
		size_t operator()(const Halley::Vector2i& v) const 
		{
			return std::hash<long long>()(*reinterpret_cast<const long long*>(&v));
		}
	};

	template<>
	struct hash<Halley::Vector2f>
	{
		size_t operator()(const Halley::Vector2f& v) const 
		{
			return std::hash<long long>()(*reinterpret_cast<const long long*>(&v));
		}
	};
}
