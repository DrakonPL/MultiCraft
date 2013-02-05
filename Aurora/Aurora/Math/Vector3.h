#ifndef AURORA_VECTOR3_H
#define AURORA_VECTOR3_H

#include <Aurora/Math/MathLib.h>

namespace Aurora
{
	namespace Math
	{
		class Vector3d
		{
		public:
			int x, y, z;
		};

		class Vector3
		{
			friend Vector3 operator*(float lhs, const Vector3 &rhs);
			friend Vector3 operator-(const Vector3 &v);

		public:
			float x, y, z;

			static Vector3 cross(const Vector3 &p, const Vector3 &q);
			static float distance(const Vector3 &pt1, const Vector3 &pt2);
			static float distanceSq(const Vector3 &pt1, const Vector3 &pt2);  
			static float dot(const Vector3 &p, const Vector3 &q);
			static Vector3 lerp(const Vector3 &p, const Vector3 &q, float t);
			static void orthogonalize(Vector3 &v1, Vector3 &v2);
			static void orthogonalize(Vector3 &v1, Vector3 &v2, Vector3 &v3);
			static Vector3 proj(const Vector3 &p, const Vector3 &q);
			static Vector3 perp(const Vector3 &p, const Vector3 &q);
			static Vector3 reflect(const Vector3 &i, const Vector3 &n);
			static Vector3 normalized(Vector3 pt1);


			static Vector3 Normal(Vector3 vTriangle[]);
			static Vector3 cross3(Vector3 p, Vector3 q);
			static Vector3 clamp(Vector3 value1,Vector3 min,Vector3 max);
			static Vector3 fromVectors(Vector3 vPoint1, Vector3 vPoint2);

			Vector3() {x= y= z= 0.0f;}
			Vector3(float x_, float y_, float z_);

			bool operator==(const Vector3 &rhs) const;
			bool operator!=(const Vector3 &rhs) const;

			Vector3 &operator+=(const Vector3 &rhs);
			Vector3 &operator*=(const Vector3 &rhs);
			Vector3 &operator-=(const Vector3 &rhs);
			Vector3 &operator*=(float scalar);
			Vector3 &operator/=(float scalar);

			Vector3 operator+(const Vector3 &rhs) const;
			Vector3 operator-(const Vector3 &rhs) const;
			Vector3 operator*(const Vector3 &rhs) const;
			Vector3 operator*(float scalar) const;
			Vector3 operator/(float scalar) const;

			float magnitude() const;
			float magnitudeSq() const;
			Vector3 inverse() const;
			Vector3 rotatedXZ(float angle);
			Vector3 rotatedZY(float angle);
			Vector3 rotatedXY(float angle);
			void normalize();
			void set(float x_, float y_, float z_);

			void saturate(void)
			    {
			        if (x < 0)
			            x = 0;
			        else if (x > 1)
			            x = 1;

			        if (y < 0)
			            y = 0;
			        else if (y > 1)
			            y = 1;

			        if (z < 0)
			            z = 0;
			        else if (z > 1)
			            z = 1;
				}
		};
	}
}

#endif
