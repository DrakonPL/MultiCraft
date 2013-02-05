#ifndef AURORA_VECTOR2_H
#define AURORA_VECTOR2_H

#include <Aurora/Math/MathLib.h>

namespace Aurora
{
	namespace Math
	{
		class Vector2
		{
			friend Vector2 operator*(float lhs, const Vector2 &rhs);
			friend Vector2 operator-(const Vector2 &v);

		public:
			float x, y;

			static float distance(const Vector2 &pt1, const Vector2 &pt2);
			static float distanceSq(const Vector2 &pt1, const Vector2 &pt2);  
			static float dot(const Vector2 &p, const Vector2 &q);
			static Vector2 lerp(const Vector2 &p, const Vector2 &q, float t);
			static void orthogonalize(Vector2 &v1, Vector2 &v2);
			static Vector2 proj(const Vector2 &p, const Vector2 &q);
			static Vector2 perp(const Vector2 &p, const Vector2 &q);
			static Vector2 reflect(const Vector2 &i, const Vector2 &n);

			Vector2() {}
			Vector2(float x_, float y_);
			~Vector2() {}

			bool operator==(const Vector2 &rhs) const;
			bool operator!=(const Vector2 &rhs) const;

			Vector2 &operator+=(const Vector2 &rhs);
			Vector2 &operator-=(const Vector2 &rhs);
			Vector2 &operator*=(float scalar);
			Vector2 &operator/=(float scalar);

			Vector2 operator+(const Vector2 &rhs) const;
			Vector2 operator-(const Vector2 &rhs) const;
			Vector2 operator*(float scalar) const;
			Vector2 operator/(float scalar) const;

			float magnitude() const;
			float magnitudeSq() const;
			Vector2 inverse() const;
			void normalize();
			void set(float x_, float y_);
		};
	}
}

#endif

