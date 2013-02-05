#ifndef AURORA_FRUSTUM_H
#define AURORA_FRUSTUM_H

#include <Aurora/Math/Vector3.h>
#include <Aurora/Math/Matrix4.h>
#include <Aurora/Math/Plane.h>
#include <Aurora/Math/BoundingBox.h>
#include <Aurora/Math/BoundingSphere.h>

namespace Aurora
{
	namespace Math
	{

		class Frustum
		{
		public:
			enum
			{
				FRUSTUM_PLANE_LEFT   = 0,
				FRUSTUM_PLANE_RIGHT  = 1,
				FRUSTUM_PLANE_BOTTOM = 2,
				FRUSTUM_PLANE_TOP    = 3,
				FRUSTUM_PLANE_NEAR   = 4,
				FRUSTUM_PLANE_FAR    = 5
			};

			enum PlaneIntersectionType { Outside = 0, Inside = 1, Intersects = 2 };

			Plane planes[6];

			void ExtractPlanes(const Matrix4 &viewMatrix4, const Matrix4 &projMatrix4);

			bool BoxInFrustumFB(BoundingBox &box);

			bool BoxInFrustum(const BoundingBox &box) const;
			bool PointInFrustum(const Vector3 &point) const;
			bool PointInFrustum(const BoundingSphere &sphere) const;
			bool PointInFrustum(int plane,const Vector3 &point,float &distance) const;
			bool SphereInFrustum(const BoundingSphere &sphere) const;

			PlaneIntersectionType BoxInFrustum(BoundingBox &box);
			PlaneIntersectionType BoxInFrustum(float minx,float miny,float minz,float maxx,float maxy,float maxz);
		};
	}
}

#endif
