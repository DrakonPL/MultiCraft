#ifndef AURORA_BOUNDINGBOX_H
#define AURORA_BOUNDINGBOX_H

#include <Aurora/Math/Vector3.h>
#include <Aurora/Math/Plane.h>


namespace Aurora
{
	namespace Math
	{
		class BoundingBox
		{
		public:
			enum PointValidity { Invalid = 0, Valid = 1 };
			enum PlaneIntersectionType { Front = 0,Back = 1,Intersecting = 2 };

			PointValidity Validity;

			Vector3 min;
			Vector3 max;

			BoundingBox();
			BoundingBox(const Vector3 &min_, const Vector3 &max_);
			BoundingBox(float minx,float miny,float minz,float maxx,float maxy,float maxz);
			~BoundingBox();

			void expandToInclude(Vector3 pt);

			//intersection
			bool contains(Vector3 pt);
			bool intersect(BoundingBox &box);
			PlaneIntersectionType intersect(Plane &plane);
			Vector3 onBorder(Vector3 pt);

			Vector3 getCenter() const;
			Vector3 getDimension() const;
			float getRadius() const;
			float getSize() const;

			//
			Vector3 ClosestPointTo(Vector3 point);
			Vector3 getFirstHitPlane(Vector3 direction, Vector3 pos, Vector3 dimensions, bool testX, bool testY, bool testZ);
		};
	}
}

#endif
