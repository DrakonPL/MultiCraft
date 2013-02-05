#include <Aurora/Math/Frustum.h>

namespace Aurora
{
	namespace Math
	{
		void Frustum::ExtractPlanes(const Matrix4 &viewMatrix, const Matrix4 &projMatrix)
		{
			// Extracts the view frustum clipping planes from the combined
			// view-projection matrix in world space. The extracted planes will
			// have their normals pointing towards the inside of the view frustum.
			//
			// References:
			//  Gil Gribb, Klaus Hartmann, "Fast Extraction of Viewing Frustum
			//  Planes from the World-View-Projection Matrix,"
			//  http://www2.ravensoft.com/users/ggribb/plane%20extraction.pdf

			Matrix4 m(viewMatrix * projMatrix);
			Plane *pPlane = 0;

			// Left clipping plane.
			pPlane = &planes[FRUSTUM_PLANE_LEFT];
			pPlane->set(
				m[0][3] + m[0][0],
				m[1][3] + m[1][0],
				m[2][3] + m[2][0],
				m[3][3] + m[3][0]);
			pPlane->normalize();

			// Right clipping plane.
			pPlane = &planes[FRUSTUM_PLANE_RIGHT];
			pPlane->set(
				m[0][3] - m[0][0],
				m[1][3] - m[1][0],
				m[2][3] - m[2][0],
				m[3][3] - m[3][0]);
			pPlane->normalize();

			// Top clipping plane.
			pPlane = &planes[FRUSTUM_PLANE_TOP];
			pPlane->set(
				m[0][3] - m[0][1],
				m[1][3] - m[1][1],
				m[2][3] - m[2][1],
				m[3][3] - m[3][1]);
			pPlane->normalize();

			// Bottom clipping plane.
			pPlane = &planes[FRUSTUM_PLANE_BOTTOM];
			pPlane->set(
				m[0][3] + m[0][1],
				m[1][3] + m[1][1],
				m[2][3] + m[2][1],
				m[3][3] + m[3][1]);
			pPlane->normalize();

			// Near clipping plane.
			pPlane = &planes[FRUSTUM_PLANE_NEAR];
			pPlane->set(m[0][2], m[1][2], m[2][2], m[3][2]);
			pPlane->normalize();

			// Far clipping plane.
			pPlane = &planes[FRUSTUM_PLANE_FAR];
			pPlane->set(
				m[0][3] - m[0][2],
				m[1][3] - m[1][2],
				m[2][3] - m[2][2],
				m[3][3] - m[3][2]);
			pPlane->normalize();
		}

		Frustum::PlaneIntersectionType Frustum::BoxInFrustum(float minx,float miny,float minz,float maxx,float maxy,float maxz)
		{
			BoundingBox box(minx,miny,minz,maxx,maxy,maxz);
			bool flag = false;

			for (int i = 0; i < 6; ++i)
			{
				switch(box.intersect(planes[i]))
				{
				  case BoundingBox::Back:
				  return Outside;

				  case BoundingBox::Front:
				  case BoundingBox::Intersecting:
					flag = true;
					break;
				}
			}

			if (!flag)
			{
				return Inside;
			}


			return Intersects;
		}

		bool Frustum::BoxInFrustumFB(BoundingBox &box)
		{
			BoundingBox::PlaneIntersectionType backTest = box.intersect(planes[FRUSTUM_PLANE_NEAR]);
			BoundingBox::PlaneIntersectionType fronTest = box.intersect(planes[FRUSTUM_PLANE_FAR]);

			if(backTest == BoundingBox::Front && fronTest == BoundingBox::Front)
			{
				return true;
			}

			return false;
		}

		Frustum::PlaneIntersectionType Frustum::BoxInFrustum(BoundingBox &box)
		{
			int flag = 0;
			for (int i = 0; i < 6; ++i)
			{
				switch(box.intersect(planes[i]))
				{
					case BoundingBox::Front:
					case BoundingBox::Intersecting:
					{
						flag = 1;
					}break;

					case BoundingBox::Back:
					return Outside;
				}
			}

			return Intersects;
		}

		bool Frustum::BoxInFrustum(const BoundingBox &box) const
		{
			Vector3 c((box.min + box.max) * 0.5f);
			float sizex = box.max.x - box.min.x;
			float sizey = box.max.y - box.min.y;
			float sizez = box.max.z - box.min.z;

			Vector3 corners[8] =
			{
				Vector3((c.x - sizex * 0.5f), (c.y - sizey * 0.5f), (c.z - sizez * 0.5f)),
				Vector3((c.x + sizex * 0.5f), (c.y - sizey * 0.5f), (c.z - sizez * 0.5f)),
				Vector3((c.x - sizex * 0.5f), (c.y + sizey * 0.5f), (c.z - sizez * 0.5f)),
				Vector3((c.x + sizex * 0.5f), (c.y + sizey * 0.5f), (c.z - sizez * 0.5f)),
				Vector3((c.x - sizex * 0.5f), (c.y - sizey * 0.5f), (c.z + sizez * 0.5f)),
				Vector3((c.x + sizex * 0.5f), (c.y - sizey * 0.5f), (c.z + sizez * 0.5f)),
				Vector3((c.x - sizex * 0.5f), (c.y + sizey * 0.5f), (c.z + sizez * 0.5f)),
				Vector3((c.x + sizex * 0.5f), (c.y + sizey * 0.5f), (c.z + sizez * 0.5f))
			};

			for (int i = 0; i < 6; ++i)
			{
				if (Plane::dot(planes[i], corners[0]) > 0.0f)
					continue;

				if (Plane::dot(planes[i], corners[1]) > 0.0f)
					continue;

				if (Plane::dot(planes[i], corners[2]) > 0.0f)
					continue;

				if (Plane::dot(planes[i], corners[3]) > 0.0f)
					continue;

				if (Plane::dot(planes[i], corners[4]) > 0.0f)
					continue;

				if (Plane::dot(planes[i], corners[5]) > 0.0f)
					continue;

				if (Plane::dot(planes[i], corners[6]) > 0.0f)
					continue;

				if (Plane::dot(planes[i], corners[7]) > 0.0f)
					continue;

				return false;
			}

			return true;
		}

		bool Frustum::PointInFrustum(const Vector3 &point) const
		{
			for (int i = 0; i < 6; ++i)
			{
				if (Plane::dot(planes[i], point) <= 0.0f)
					return false;
			}

			return true;
		}

		bool Frustum::PointInFrustum(int plane,const Vector3 &point,float &distance) const
		{
			distance = Plane::dot(planes[plane], point);
			if (distance <= 0.0f)
				return false;

			return true;
		}

		bool Frustum::SphereInFrustum(const BoundingSphere &sphere) const
		{
			for (int i = 0; i < 6; ++i)
			{
				if (Plane::dot(planes[i], sphere.center) <= -sphere.radius)
					return false;
			}

			return true;
		}

		

	}
}

