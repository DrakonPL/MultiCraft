#include <Aurora/Math/Vector3.h>
#include <Aurora/Math/Vector4.h>
#include <Aurora/Math/MathUtils.h>

namespace Aurora
{
	namespace Math
	{
		Vector4 operator*(float lhs, const Vector4 &rhs)
		{
			return Vector4(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w);
		}

		Vector4 operator-(const Vector4 &v)
		{
			return Vector4(-v.x, -v.y, -v.z, -v.w);
		}

		float Vector4::distance(const Vector4 &pt1, const Vector4 &pt2)
		{
			// Calculates the distance between 2 points.
			return ASqrtf(distanceSq(pt1, pt2));
		}

		float Vector4::distanceSq(const Vector4 &pt1, const Vector4 &pt2)
		{
			// Calculates the squared distance between 2 points.
			return ((pt1.x - pt2.x) * (pt1.x - pt2.x))
				+ ((pt1.y - pt2.y) * (pt1.y - pt2.y))
				+ ((pt1.z - pt2.z) * (pt1.z - pt2.z))
				+ ((pt1.w - pt2.w) * (pt1.w - pt2.w));
		}

		float Vector4::dot(const Vector4 &p, const Vector4 &q)
		{
			return (p.x * q.x) + (p.y * q.y) + (p.z * q.z) + (p.w * q.w);
		}

		Vector4 Vector4::lerp(const Vector4 &p, const Vector4 &q, float t)
		{
			// Linearly interpolates from 'p' to 'q' as t varies from 0 to 1.
			return p + t * (q - p);
		}

		Vector4::Vector4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}

		Vector4::Vector4(const Vector3 &v, float w_) : x(v.x), y(v.y), z(v.z), w(w_) {}

		Vector4 &Vector4::operator+=(const Vector4 &rhs)
		{
			x += rhs.x, y += rhs.y, z += rhs.z, w += rhs.w;
			return *this;
		}

		bool Vector4::operator==(const Vector4 &rhs) const
		{
			return Math::closeEnough(x, rhs.x) && Math::closeEnough(y, rhs.y)
				&& Math::closeEnough(z, rhs.z) && Math::closeEnough(w, rhs.w);
		}

		bool Vector4::operator!=(const Vector4 &rhs) const
		{
			return !(*this == rhs);
		}

		Vector4 &Vector4::operator-=(const Vector4 &rhs)
		{
			x -= rhs.x, y -= rhs.y, z -= rhs.z, w -= rhs.w;
			return *this;
		}

		Vector4 &Vector4::operator*=(float scalar)
		{
			x *= scalar, y *= scalar, z *= scalar, w *= scalar;
			return *this;
		}

		Vector4 &Vector4::operator/=(float scalar)
		{
			x /= scalar, y /= scalar, z /= scalar, w /= scalar;
			return *this;
		}

		Vector4 Vector4::operator+(const Vector4 &rhs) const
		{
			Vector4 tmp(*this);
			tmp += rhs;
			return tmp;
		}

		Vector4 Vector4::operator-(const Vector4 &rhs) const
		{
			Vector4 tmp(*this);
			tmp -= rhs;
			return tmp;
		}

		Vector4 Vector4::operator*(float scalar) const
		{
			return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
		}

		Vector4 Vector4::operator/(float scalar) const
		{
			return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
		}

		float Vector4::magnitude() const
		{
			return ASqrtf((x * x) + (y * y) + (z * z) + (w * w));
		}

		float Vector4::magnitudeSq() const
		{
			return (x * x) + (y * y) + (z * z) + (w * w);
		}

		Vector4 Vector4::inverse() const
		{
			return Vector4(-x, -y, -z, -w);
		}

		void Vector4::normalize()
		{
			float invMag = 1.0f / magnitude();
			x *= invMag, y *= invMag, z *= invMag, w *= invMag;
		}

		void Vector4::set(float x_, float y_, float z_, float w_)
		{
			x = x_, y = y_, z = z_, w = w_;
		}

		Vector3 Vector4::toVector3() const
		{
			return (w != 0.0f) ? Vector3(x / w, y / w, z / w) : Vector3(x, y, z);
		}
	}
}
