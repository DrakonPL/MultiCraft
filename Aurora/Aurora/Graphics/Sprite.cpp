#include <Aurora/Graphics/Sprite.h>
#include <Aurora/Graphics/TextureManager.h>

namespace Aurora
{

	namespace Graphics
	{

		Sprite::Sprite()
		{
			imageName = "";
			vertices = 0;
		}

		TexturedVertex getVertex(float u,float v,float x, float y,float z)
		{
			TexturedVertex vert;
			vert.u = u;
			vert.v = v;
			vert.x = x;
			vert.y = y;
			vert.z = z;
			return vert;
		}

		Sprite::Sprite(const char* filename)
		{
			imageName = filename;
			TextureManager::Instance()->loadImageFromFile(filename);

			//generate wertices
			vertices = new TexturedVertex[4];

			Image *img = TextureManager::Instance()->getImage(filename);

			width = img->_width;
			height = img->_height;

			float hPercent = (float)img->_width / (float)img->_height;
			float wPercent = (float)img->_width / (float)img->_width;

			if( vertices )
			{
				vertices[0] = getVertex(0.0f,0.0f,-img->_width/2.0f,-img->_height/2.0f,0.0f);
				vertices[1] = getVertex(0.0f,hPercent,-img->_width/2.0f, img->_height/2.0f,0.0f);
				vertices[2] = getVertex(wPercent,0.0f,img->_width/2.0f,-img->_height/2.0f,0.0f);
				vertices[3] = getVertex(wPercent,hPercent,img->_width/2.0f, img->_height/2.0f,0.0f);
			}
		}

		Sprite::Sprite(const char* filename,int startW,int startH,int endW,int endH)
		{
			imageName = filename;
			TextureManager::Instance()->loadImageFromFile(filename);

			//generate wertices
			vertices = new TexturedVertex[4];

			Image* img = TextureManager::Instance()->getImage(filename);

			width = endW;
			height = endH;

			float hstart = (float)startH / (float)img->_height;
			float wstart = (float)startW / (float)img->_width;
			float hPercent = (float)(startH + endH) / (float)img->_height;
			float wPercent = (float)(startW + endW) / (float)img->_width;

			if( vertices )
			{
				vertices[0] = getVertex(wstart,hstart,-width/2.0f,-height/2.0f,0.0f);
				vertices[1] = getVertex(wstart,hPercent,-width/2.0f, height/2.0f,0.0f);
				vertices[2] = getVertex(wPercent,hstart,width/2.0f,-height/2.0f,0.0f);
				vertices[3] = getVertex(wPercent,hPercent,width/2.0f, height/2.0f,0.0f);
			}

		}

		Sprite::~Sprite()
		{
			if(vertices != 0)
				delete [] vertices;
		}

		void Sprite::SetPosition(float x,float y)
		{
			posX = x;
			posY = y;
		}

		void Sprite::Scale(float x,float y)
		{
			scaleX = x;
			scaleY = y;

			for(int i=0; i<4; i++)
			{
				vertices[i].x = vertices[i].x * scaleX;
				vertices[i].y = vertices[i].y * scaleY;
			}

		}

		void Sprite::RemoveImage()
		{
			TextureManager::Instance()->removeImage(imageName);
		}

	}

}

