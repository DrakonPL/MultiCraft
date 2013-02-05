#ifndef CAMERA_H
#define CAMERA_H

#include <Aurora/Math/MathLib.h>
#include <Aurora/Math/Vector3.h>
#include <Aurora/Math/Matrix4.h>
#include <Aurora/Math/Quaternion.h>
#include <Aurora/Math/Frustum.h>

using namespace Aurora::Math;

namespace Aurora
{
	namespace Graphics
	{
		class Camera
		{
		public:

			Vector3 m_vPosition;
			
			// The camera's offset
			Vector3 m_vOffset;

			// The camera's view
			Vector3 m_vView;

			// The camera's up vector
			Vector3 m_vUpVector;

			// The camera's strafe vector
			Vector3 m_vStrafe;

			//TODO wtf is this for??
			Vector3 m_vVelocity;

			Vector3 vVector;

			float upDownAngle;

			Frustum* camFrustum;

		public:

			Camera();
			~Camera();

			void PositionCamera(float positionX, float positionY, float positionZ,
						 		    float viewX,     float viewY,     float viewZ,
									float upVectorX, float upVectorY, float upVectorZ);

			void Move(float speed);
			void Strafe(float speed);
			void RotateView(float angle, float x, float y, float z);
			void PitchView(float speed);

			void SetPosition(Vector3 position);

			inline Vector3 GetViewDirection() { return (m_vView - m_vPosition);}
			inline bool NeedUpdate() { return needUpdate;}

			bool needUpdate;
		};
	}
}

#endif
