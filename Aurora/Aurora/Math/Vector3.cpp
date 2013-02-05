#include <Aurora/Math/Vector3.h>
#include <Aurora/Math/MathUtils.h>

namespace Aurora
{
	namespace Math
	{
		Vector3 Vector3::Normal(Vector3 vTriangle[])
		{														// Get 2 vectors from the polygon (2 sides), Remember the order!
			Vector3 vVector1 = vTriangle[2]- vTriangle[0];
			Vector3 vVector2 = vTriangle[1]- vTriangle[0];

			Vector3 vNormal = Vector3::cross3(vVector1, vVector2);		// Take the cross product of our 2 vectors to get a perpendicular vector

			vNormal.normalize();								// Use our function we created to normalize the normal (Makes it a length of one)

			return vNormal;										// Return our normal at our desired length
		}

		Vector3 Vector3::cross3(Vector3 p, Vector3 q)
		{
			return Vector3((p.y * q.z) - (p.z * q.y),
				(p.z * q.x) - (p.x * q.z),
				(p.x * q.y) - (p.y * q.x));
		}

		Vector3 Vector3::clamp(Vector3 value1,Vector3 min,Vector3 max)
		{
			Vector3 result;

			float x = value1.x;
			x = (x > max.x) ? max.x : x;
			x = (x < min.x) ? min.x : x;
			float y = value1.y;
			y = (y > max.y) ? max.y : y;
			y = (y < min.y) ? min.y : y;
			float z = value1.z;
			z = (z > max.z) ? max.z : z;
			z = (z < min.z) ? min.z : z;
			result.x = x;
			result.y = y;
			result.z = z;

			return result;
		}

		Vector3 Vector3::fromVectors(Vector3 vPoint1, Vector3 vPoint2)
		{
			Vector3 vVector;								// Initialize our variable to zero

			vVector.x = vPoint1.x - vPoint2.x;					// Get the X value of our new vector
			vVector.y = vPoint1.y - vPoint2.y;					// Get the Y value of our new vector
			vVector.z = vPoint1.z - vPoint2.z;					// Get the Z value of our new vector

			return vVector;										// Return our new vector
		}

		Vector3 operator*(float lhs, const Vector3 &rhs)
		{
			return Vector3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
		}

		Vector3 operator-(const Vector3 &v)
		{
			return Vector3(-v.x, -v.y, -v.z);
		}

		Vector3 Vector3::cross(const Vector3 &p, const Vector3 &q)
		{
			return Vector3((p.y * q.z) - (p.z * q.y),
				(p.z * q.x) - (p.x * q.z),
				(p.x * q.y) - (p.y * q.x));
		}

		float Vector3::distance(const Vector3 &pt1, const Vector3 &pt2)
		{
			// Calculates the distance between 2 points.
			float f = distanceSq(pt1, pt2);
			return ASqrtf(f);
		}

		float Vector3::distanceSq(const Vector3 &pt1, const Vector3 &pt2)
		{
			// Calculates the squared distance between 2 points.
			return ((pt1.x - pt2.x) * (pt1.x - pt2.x))+ ((pt1.y - pt2.y) * (pt1.y - pt2.y))	+ ((pt1.z - pt2.z) * (pt1.z - pt2.z));
		}

		float Vector3::dot(const Vector3 &p, const Vector3 &q)
		{
			return (p.x * q.x) + (p.y * q.y) + (p.z * q.z);
		}

		Vector3 Vector3::lerp(const Vector3 &p, const Vector3 &q, float t)
		{
			// Linearly interpolates from 'p' to 'q' as t varies from 0 to 1.
			return p + t * (q - p);
		}

		void Vector3::orthogonalize(Vector3 &v1, Vector3 &v2)
		{
			// Performs Gram-Schmidt Orthogonalization on the 2 basis vectors to
			// turn them into orthonormal basis vectors.
			v2 = v2 - proj(v2, v1);
			v2.normalize();
		}

		void Vector3::orthogonalize(Vector3 &v1, Vector3 &v2, Vector3 &v3)
		{
			// Performs Gram-Schmidt Orthogonalization on the 3 basis vectors to
			// turn them into orthonormal basis vectors.

			v2 = v2 - proj(v2, v1);
			v2.normalize();

			v3 = v3 - proj(v3, v1) - proj(v3, v2);
			v3.normalize();
		}

		Vector3 Vector3::proj(const Vector3 &p, const Vector3 &q)
		{
			// Calculates the projection of 'p' onto 'q'.
			float length =  q.magnitude();
			return (Vector3::dot(p, q) / (length * length)) * q;
		}

		Vector3 Vector3::perp(const Vector3 &p, const Vector3 &q)
		{
			// Calculates the component of 'p' perpendicular to 'q'.
			float length = q.magnitude();
			return p - ((Vector3::dot(p, q) / (length * length)) * q);
		}

		Vector3 Vector3::reflect(const Vector3 &i, const Vector3 &n)
		{
			// Calculates reflection vector from entering ray direction 'i'
			// and surface normal 'n'.
			return i - 2.0f * Vector3::proj(i, n);
		}

		Vector3 Vector3::normalized(Vector3 pt1)
		{
			Vector3 temp = pt1;
			temp.normalize();
			return temp;
		}

		Vector3 Vector3::rotatedXZ(float angle)
		{
			Vector3 temp;

			temp.x = (x * cosf(angle)) - (z * sinf(angle));
			temp.y = y;
			temp.z = (z * cosf(angle)) + (x * sinf(angle));

			return temp;
		}

		Vector3 Vector3::rotatedZY(float angle)
		{
			Vector3 temp;

			temp.x = x;
			temp.y = (y * cosf(angle)) + (z * sinf(angle));
			temp.z = (z * cosf(angle)) - (y * sinf(angle));

			return temp;
		}

		Vector3 Vector3::rotatedXY(float angle)
		{
			Vector3 temp;

			temp.x = (x * cosf(angle)) - (y * sinf(angle));
			temp.y = (y * cosf(angle)) + (x * sinf(angle));;
			temp.z = z;

			return temp;
		}


		Vector3::Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

		Vector3 &Vector3::operator+=(const Vector3 &rhs)
		{
			x += rhs.x, y += rhs.y, z += rhs.z;
			return *this;
		}

		Vector3 &Vector3::operator*=(const Vector3 &rhs)
		{
			x *= rhs.x, y *= rhs.y, z *= rhs.z;
			return *this;
		}

		bool Vector3::operator==(const Vector3 &rhs) const
		{
			return Math::closeEnough(x, rhs.x) && Math::closeEnough(y, rhs.y) && Math::closeEnough(z, rhs.z);
		}

		bool Vector3::operator!=(const Vector3 &rhs) const
		{
			return !(*this == rhs);
		}

		Vector3 &Vector3::operator-=(const Vector3 &rhs)
		{
			x -= rhs.x, y -= rhs.y, z -= rhs.z;
			return *this;
		}

		Vector3 &Vector3::operator*=(float scalar)
		{
			x *= scalar, y *= scalar, z *= scalar;
			return *this;
		}

		Vector3 &Vector3::operator/=(float scalar)
		{
			x /= scalar, y /= scalar, z /= scalar;
			return *this;
		}

		Vector3 Vector3::operator+(const Vector3 &rhs) const
		{
			Vector3 tmp(*this);
			tmp += rhs;
			return tmp;
		}

		Vector3 Vector3::operator*(const Vector3 &rhs) const
		{
			Vector3 tmp(*this);
			tmp *= rhs;
			return tmp;
		}

		Vector3 Vector3::operator-(const Vector3 &rhs) const
		{
			Vector3 tmp(*this);
			tmp -= rhs;
			return tmp;
		}

		Vector3 Vector3::operator*(float scalar) const
		{
			return Vector3(x * scalar, y * scalar, z * scalar);    
		}

		Vector3 Vector3::operator/(float scalar) const
		{
			return Vector3(x / scalar, y / scalar, z / scalar);
		}

		float Vector3::magnitude() const
		{
			return ASqrtf((x * x) + (y * y) + (z * z));
		}

		float Vector3::magnitudeSq() const
		{
			return (x * x) + (y * y) + (z * z);
		}

		Vector3 Vector3::inverse() const
		{
			return Vector3(-x, -y, -z);
		}

		void Vector3::normalize()
		{
			float m = magnitude();
			if(m != 0.0f)
			{
				float invMag = 1.0f / m;
				x *= invMag, y *= invMag, z *= invMag;
			}
		}

		void Vector3::set(float x_, float y_, float z_)
		{
			x = x_, y = y_, z = z_;
		}
	}
}
