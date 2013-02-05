#ifndef RENDERTEXTURE_H
#define RENDERTEXTURE_H

#include <Aurora/Graphics/Image.h>

#include <string>

namespace Aurora
{
	namespace Graphics
	{
		class RenderTexture
		{
		private:

			Image* _image;

		public:

			//frame buffer object id
			unsigned int _fboId;
			//render buffer object id
			unsigned int _rboId;

			std::string _status;

		public:

			RenderTexture(std::string name,int width,int height,ImageLocation location);
			~RenderTexture();

			inline Image* GetImage() { return _image; }
		};
	}
}

#endif
