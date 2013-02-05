#ifndef BOUNDINGBOX2D_H
#define BOUNDINGBOX2D_H

#include <Aurora/Math/Vector2.h>

namespace Aurora
{
	namespace Math
	{
		class BoundingBox2D
		{
		public:
			enum PointValidity { Invalid = 0, Valid = 1 };
			enum PlaneIntersectionType { Front = 0,Back = 1,Intersecting = 2 };

			PointValidity Validity;

			Vector2 min;
			Vector2 max;

			BoundingBox2D();
			BoundingBox2D(const Vector2 &min_, const Vector2 &max_);
			BoundingBox2D(float minx,float miny,float maxx,float maxy);
			~BoundingBox2D();

			void expandToInclude(Vector2 pt);
			void Reset();

			//intersection
			bool contains(Vector2 pt);
			bool intersect(BoundingBox2D &box);

			Vector2 getCenter();
			float getRadius();
			float getSize();
		};
	}
}

#endif
