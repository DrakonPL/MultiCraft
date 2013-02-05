#ifndef AURORA_VECTOR4_H
#define AURORA_VECTOR4_H

#include <Aurora/Math/MathLib.h>


namespace Aurora
{
	namespace Math
	{
		class Vector4
		{
			friend Vector4 operator*(float lhs, const Vector4 &rhs);
			friend Vector4 operator-(const Vector4 &v);

		public:
			float x, y, z, w;

			static float distance(const Vector4 &pt1, const Vector4 &pt2);
			static float distanceSq(const Vector4 &pt1, const Vector4 &pt2);
			static float dot(const Vector4 &p, const Vector4 &q);
			static Vector4 lerp(const Vector4 &p, const Vector4 &q, float t);

			Vector4() {}
			Vector4(float x_, float y_, float z_, float w_);
			Vector4(const Vector3 &v, float w_);
			~Vector4() {}

			bool operator==(const Vector4 &rhs) const;
			bool operator!=(const Vector4 &rhs) const;

			Vector4 &operator+=(const Vector4 &rhs);
			Vector4 &operator-=(const Vector4 &rhs);
			Vector4 &operator*=(float scalar);
			Vector4 &operator/=(float scalar);

			Vector4 operator+(const Vector4 &rhs) const;
			Vector4 operator-(const Vector4 &rhs) const;
			Vector4 operator*(float scalar) const;
			Vector4 operator/(float scalar) const;

			float magnitude() const;
			float magnitudeSq() const;
			Vector4 inverse() const;
			void normalize();
			void set(float x_, float y_, float z_, float w_);
			Vector3 toVector3() const;
		};
	}
}

#endif
