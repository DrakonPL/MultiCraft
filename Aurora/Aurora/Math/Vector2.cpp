#include <Aurora/Math/Vector2.h>
#include <Aurora/Math/MathUtils.h>

namespace Aurora
{
	namespace Math
	{
		Vector2 operator*(float lhs, const Vector2 &rhs)
		{
			return Vector2(lhs * rhs.x, lhs * rhs.y);
		}

		Vector2 operator-(const Vector2 &v)
		{
			return Vector2(-v.x, -v.y);
		}

		float Vector2::distance(const Vector2 &pt1, const Vector2 &pt2)
		{
			// Calculates the distance between 2 points.
			return ASqrtf(distanceSq(pt1, pt2));
		}

		float Vector2::distanceSq(const Vector2 &pt1, const Vector2 &pt2)
		{
			// Calculates the squared distance between 2 points.
			return ((pt1.x - pt2.x) * (pt1.x - pt2.x)) + ((pt1.y - pt2.y) * (pt1.y - pt2.y));
		}

		float Vector2::dot(const Vector2 &p, const Vector2 &q)
		{
			return (p.x * q.x) + (p.y * q.y);
		}

		Vector2 Vector2::lerp(const Vector2 &p, const Vector2 &q, float t)
		{
			// Linearly interpolates from 'p' to 'q' as t varies from 0 to 1.
			return p + t * (q - p);
		}

		void Vector2::orthogonalize(Vector2 &v1, Vector2 &v2)
		{
			// Performs Gram-Schmidt Orthogonalization on the 2 basis vectors to
			// turn them into orthonormal basis vectors.
			v2 = v2 - proj(v2, v1);
			v2.normalize();
		}

		Vector2 Vector2::proj(const Vector2 &p, const Vector2 &q)
		{
			// Calculates the projection of 'p' onto 'q'.
			float length =  q.magnitude();
			return (Vector2::dot(p, q) / (length * length)) * q;
		}

		Vector2 Vector2::perp(const Vector2 &p, const Vector2 &q)
		{
			// Calculates the component of 'p' perpendicular to 'q'.
			float length = q.magnitude();
			return p - ((Vector2::dot(p, q) / (length * length)) * q);
		}

		Vector2 Vector2::reflect(const Vector2 &i, const Vector2 &n)
		{
			// Calculates reflection vector from entering ray direction 'i'
			// and surface normal 'n'.
			return i - 2.0f * Vector2::proj(i, n);
		}

		Vector2::Vector2(float x_, float y_) : x(x_), y(y_) {}

		bool Vector2::operator==(const Vector2 &rhs) const
		{
			return Math::closeEnough(x, rhs.x) && Math::closeEnough(y, rhs.y);
		}

		bool Vector2::operator!=(const Vector2 &rhs) const
		{
			return !(*this == rhs);
		}

		Vector2 &Vector2::operator+=(const Vector2 &rhs)
		{
			x += rhs.x, y += rhs.y;
			return *this;
		}

		Vector2 &Vector2::operator-=(const Vector2 &rhs)
		{
			x -= rhs.x, y -= rhs.y;
			return *this;
		}

		Vector2 &Vector2::operator*=(float scalar)
		{
			x *= scalar, y *= scalar;
			return *this;
		}

		Vector2 &Vector2::operator/=(float scalar)
		{
			x /= scalar, y /= scalar;
			return *this;
		}

		Vector2 Vector2::operator+(const Vector2 &rhs) const
		{
			Vector2 tmp(*this);
			tmp += rhs;
			return tmp;
		}

		Vector2 Vector2::operator-(const Vector2 &rhs) const
		{
			Vector2 tmp(*this);
			tmp -= rhs;
			return tmp;
		}

		Vector2 Vector2::operator*(float scalar) const
		{
			return Vector2(x * scalar, y * scalar);
		}

		Vector2 Vector2::operator/(float scalar) const
		{
			return Vector2(x / scalar, y / scalar);
		}

		float Vector2::magnitude() const
		{
			return ASqrtf((x * x) + (y * y));
		}

		float Vector2::magnitudeSq() const
		{
			return (x * x) + (y * y);
		}

		Vector2 Vector2::inverse() const
		{
			return Vector2(-x, -y);
		}

		void Vector2::normalize()
		{
			float invMag = 1.0f / magnitude();
			x *= invMag, y *= invMag;
		}

		void Vector2::set(float x_, float y_)
		{
			x = x_, y = y_;
		}
	}
}
