#include <Aurora/Math/BoundingBox.h>

#include <limits>


template <typename T> int sgn(T val) {
	return (val > T(0)) - (val < T(0));
}


namespace Aurora
{
	namespace Math
	{
		BoundingBox::BoundingBox()
		{
			min.set(0.0f, 0.0f, 0.0f);
			max.set(0.0f, 0.0f, 0.0f);
			Validity = BoundingBox::Invalid;
		}

		BoundingBox::BoundingBox(const Vector3 &min_, const Vector3 &max_)
		{
			min = min_;
			max = max_;
			Validity = BoundingBox::Valid;
		}

		BoundingBox::BoundingBox(float minx,float miny,float minz,float maxx,float maxy,float maxz)
		{
			min.x = minx;
			min.y = miny;
			min.z = minz;

			max.x = maxx;
			max.y = maxy;
			max.z = maxz;

			Validity = BoundingBox::Valid;
		}

		BoundingBox::~BoundingBox()
		{
		}

		void BoundingBox::expandToInclude(Vector3 pt)
		{
			if (Validity == BoundingBox::Valid)
			{
				if (pt.x < min.x) { min.x = pt.x; }
				else if (pt.x > max.x) { max.x = pt.x; }

				if (pt.y < min.y) { min.y = pt.y; }
				else if (pt.y > max.y) { max.y = pt.y; }

				if (pt.z < min.z) { min.z = pt.z; }
				else if (pt.z > max.z) { max.z = pt.z; }
			}
			else
			{
				min = max = pt;
				Validity = BoundingBox::Valid;
			}
		}

		bool BoundingBox::contains(Vector3 pt)
		{
			return ((pt.x >= min.x) && (pt.x <= max.x) && (pt.y >= min.y) && (pt.y <= max.y) && (pt.z >= min.z) && (pt.z <= max.z));
		}

		bool BoundingBox::intersect(BoundingBox &box)
		{
			if (!((min.x <= box.max.x) && (max.x >= box.min.x)))
				return false;

			if (!((min.y <= box.max.y) && (max.y >= box.min.y)))
				return false;

			return ((min.z <= box.max.z) && (max.z >= box.min.z));
		}

		BoundingBox::PlaneIntersectionType BoundingBox::intersect(Plane &plane)
		{
			Vector3 vector;
			Vector3 vector2;
			vector2.x = (plane.n.x >= 0.0f) ? min.x : max.x;
			vector2.y = (plane.n.y >= 0.0f) ? min.y : max.y;
			vector2.z = (plane.n.z >= 0.0f) ? min.z : max.z;
			vector.x = (plane.n.x >= 0.0f) ? max.x : min.x;
			vector.y = (plane.n.y >= 0.0f) ? max.y : min.y;
			vector.z = (plane.n.z >= 0.0f) ? max.z : min.z;


			float num = Vector3::dot(plane.n,vector2);//((plane.n.x * vector2.x) + (plane.n.y * vector2.y)) + (plane.n.z * vector2.z);
			if ((num + plane.d) > 0.0f)
			{
				return Front;
			}

			num = Vector3::dot(plane.n,vector);//((plane.n.x * vector.x) + (plane.n.y * vector.y)) + (plane.n.z * vector.z);
			if ((num + plane.d) < 0.0f)
			{
				return Back;
			}

			return Intersecting;
		}

		Vector3 BoundingBox::onBorder(Vector3 pt)
		{
			Vector3 result(0,0,0);

			float x = min.x - pt.x;
			if(x == 0.0f)
				result.x = -1.0f;

			x = max.x - pt.x;
			if(x == 0.0f)
				result.x = 1.0f;

			float y =  min.y - pt.y;
			if(y == 0.0f)
				result.y = -1.0f;

			y = max.y - pt.y;
			if(y == 0.0f)
				result.y = 1.0f;

			float z =  min.z - pt.z;
			if(z == 0.0f)
				result.z = -1.0f;

			z = max.z - pt.z;
			if(z == 0.0f)
				result.z = 1.0f;


			return result;
		}

		Vector3 BoundingBox::getCenter() const
		{
			return (min + max) * 0.5f;
		}

		Vector3 BoundingBox::getDimension() const
		{
			return (max - min) * 0.5f;
		}

		float BoundingBox::getRadius() const
		{
			return getSize() * 0.5f;
		}

		float BoundingBox::getSize() const
		{
			return (max - min).magnitude();
		}

		Vector3 BoundingBox::ClosestPointTo(Vector3 point)
		{
			Vector3 r = point;

			if (point.x < min.x) r.x = min.x;
			if (point.x  >max.x) r.x = max.x;
			if (point.y < min.y) r.y = min.y;
			if (point.y > max.y) r.y = max.y;
			if (point.z < min.z) r.z = min.z;
			if (point.z > max.z) r.z = max.z;

			return r;
		}

		Vector3 BoundingBox::getFirstHitPlane(Vector3 direction, Vector3 pos, Vector3 dimensions, bool testX, bool testY, bool testZ)
		{
			Vector3 hitNormal;

			double dist = std::numeric_limits<double>::max( );

			if (testX)
			{
				double distX = 0;

				if (direction.x > 0)
				{
					distX = (getCenter().x - pos.x - dimensions.x - getDimension().x) / direction.x;
				}

				if (direction.x < 0)
				{
					distX = (getCenter().x - pos.x + dimensions.x + getDimension().x) / direction.x;
				}

				if (distX >= 0 && distX < dist)
				{
					hitNormal = Vector3(1 * sgn(direction.x),0,0);
				}
			}
			if (testY)
			{
				double distY = 0;
				if (direction.y > 0)
				{
					distY = (getCenter().y - pos.y - dimensions.y - getDimension().y) / direction.y;
				}
				if (direction.y < 0)
				{
					distY = (getCenter().y - pos.y + dimensions.y + getDimension().y) / direction.y;
				}

				if (distY >= 0 && distY < dist)
				{
					hitNormal = Vector3(0,1 * sgn(direction.y),0);
				}
			}
			if (testZ)
			{
				double distZ = 0;
				if (direction.z > 0)
				{
					distZ = (getCenter().z - pos.z - dimensions.z - getDimension().z) / direction.z;
				}
				if (direction.z < 0)
				{
					distZ = (getCenter().z - pos.z + dimensions.z + getDimension().z) / direction.z;
				}
				if (distZ >= 0 && distZ < dist)
				{
					hitNormal = Vector3(0,0,1 * sgn(direction.z));
				}
			}

			return hitNormal;
		}
	}
}

