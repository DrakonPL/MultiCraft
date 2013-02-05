#ifndef SPRITE_H_
#define SPRITE_H_

#include <Aurora/Graphics/Vertex.h>
#include <string>

namespace Aurora
{
	namespace Graphics
	{

		class Sprite
		{
		public:
			std::string imageName;
			TexturedVertex *vertices;

			float scaleX,scaleY;
			float posX,posY;
			int width,height;

		public:
					
			Sprite();
			Sprite(const char* filename);
			Sprite(const char* filename,int startW,int startH,int endW,int endH);
			~Sprite();

			void SetPosition(float x,float y);
			void Scale(float x,float y);
			void RemoveImage();
		};

	}

}

#endif