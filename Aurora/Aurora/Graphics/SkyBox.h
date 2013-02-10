#ifndef SKYBOX_H 
#define SKYBOX_H

#include <Aurora/Graphics/Image.h>
#include <Aurora/Math/Vector3.h>

using namespace Aurora::Math;

namespace Aurora
{
	namespace Graphics
	{
		enum SkyFace
		{
			Front,
			Back,
			Left,
			Right,
			Up,
			Down
		};

		class SkyBox
		{
		private:

			Image* _front;
			Image* _back;
			Image* _left;
			Image* _right;
			Image* _up;
			Image* _down;

			Vector3 _position;
			Vector3 _size;

		public:

			SkyBox();
			~SkyBox();

			void LoadImage(const char* fileName,SkyFace skyFace);
			void SetPositionSize(Vector3 position,Vector3 size);

			void Draw();
		};
	}
}

#endif