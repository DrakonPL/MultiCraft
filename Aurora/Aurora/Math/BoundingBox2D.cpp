#include <Aurora/Math/BoundingBox2D.h>

namespace Aurora
{
	namespace Math
	{
		BoundingBox2D::BoundingBox2D()
		{
			min.set(0.0f, 0.0f);
			max.set(0.0f, 0.0f);
			Validity = BoundingBox2D::Invalid;
		}

		BoundingBox2D::BoundingBox2D(const Vector2 &min_, const Vector2 &max_)
		{
			min = min_;
			max = max_;
			Validity = BoundingBox2D::Valid;
		}

		BoundingBox2D::BoundingBox2D(float minx,float miny,float maxx,float maxy)
		{
			min.x = minx;
			min.y = miny;

			max.x = maxx;
			max.y = maxy;

			Validity = BoundingBox2D::Valid;
		}

		BoundingBox2D::~BoundingBox2D()
		{

		}

		void BoundingBox2D::expandToInclude(Vector2 pt)
		{
			if (Validity == BoundingBox2D::Valid)
			{
				if (pt.x < min.x) { min.x = pt.x; }
				else if (pt.x > max.x) { max.x = pt.x; }

				if (pt.y < min.y) { min.y = pt.y; }
				else if (pt.y > max.y) { max.y = pt.y; }
			}
			else
			{
				min = max = pt;
				Validity = BoundingBox2D::Valid;
			}
		}

		void BoundingBox2D::Reset()
		{
			min.set(0.0f, 0.0f);
			max.set(0.0f, 0.0f);
			Validity = BoundingBox2D::Invalid;
		}

		//intersection
		bool BoundingBox2D::contains(Vector2 pt)
		{
			return ((pt.x >= min.x) && (pt.x <= max.x) && (pt.y >= min.y) && (pt.y <= max.y));
		}
		bool BoundingBox2D::intersect(BoundingBox2D &box)
		{
			if (!((min.x <= box.max.x) && (max.x >= box.min.x)))
				return false;

			if (!((min.y <= box.max.y) && (max.y >= box.min.y)))
				return false;

			return true;
		}

		Vector2 BoundingBox2D::getCenter()
		{
			return (min + max) * 0.5f;
		}

		float BoundingBox2D::getRadius()
		{
			return getSize() * 0.5f;
		}

		float BoundingBox2D::getSize()
		{
			return (max - min).magnitude();
		}
	}
}

