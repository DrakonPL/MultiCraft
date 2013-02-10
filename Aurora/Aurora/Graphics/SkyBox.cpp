#include <Aurora/Graphics/SkyBox.h>
#include <Aurora/Graphics/RenderManager.h>
#include <Aurora/Graphics/TextureManager.h>

namespace Aurora
{
	namespace Graphics
	{
		SkyBox::SkyBox()
		{
			_position.set(0.0f,0.0f,0.0f);
			_size.set(1.0f,1.0f,1.0f);

			_position = _position - _size / 2;

			_front = 0;
			_back = 0;
			_left = 0;
			_right = 0;
			_up = 0;
			_down = 0;
		}

		SkyBox::~SkyBox()
		{
			if (_front != 0)
			{
				delete _front;
			}

			if (_back != 0)
			{
				delete _back;
			}

			if (_left != 0)
			{
				delete _left;
			}

			if (_right != 0)
			{
				delete _right;
			}

			if (_up != 0)
			{
				delete _up;
			}

			if (_down != 0)
			{
				delete _down;
			}
		}

		void SkyBox::LoadImage(const char* fileName,SkyFace skyFace)
		{
			switch (skyFace)
			{
			case Aurora::Graphics::Front:
				{
					_front = new Image();
					_front = TextureManager::Instance()->loadImageFromFile(fileName);
				}
				break;
			case Aurora::Graphics::Back:
				{
					_back = new Image();
					_back = TextureManager::Instance()->loadImageFromFile(fileName);
				}
				break;
			case Aurora::Graphics::Left:
				{
					_left = new Image();
					_left = TextureManager::Instance()->loadImageFromFile(fileName);
				}
				break;
			case Aurora::Graphics::Right:
				{
					_right = new Image();
					_right = TextureManager::Instance()->loadImageFromFile(fileName);
				}
				break;
			case Aurora::Graphics::Up:
				{
					_up = new Image();
					_up = TextureManager::Instance()->loadImageFromFile(fileName);
				}
				break;
			case Aurora::Graphics::Down:
				{
					_down = new Image();
					_down = TextureManager::Instance()->loadImageFromFile(fileName);
				}
				break;
			default:
				break;
			}
		}

		void SkyBox::SetPositionSize(Vector3 position,Vector3 size)
		{
			_position = position;
			_size = size;

			_position = _position - _size / 2;
		}

		void SkyBox::Draw()
		{
			RenderManager::Instance()->SetColor(0xffffffff);

			RenderManager::Instance()->SetDepthTest(false);
			RenderManager::Instance()->SetDepthMask(false);

			//back
			{
				TexturedVertex* vertices = (TexturedVertex*)RenderManager::Instance()->CreateVertexObject(Textured,4);

				vertices[0].u = 0.0f; vertices[0].v = 1.0f;vertices[0].x =_position.x			;vertices[0].y =_position.y				;vertices[0].z =_position.z;
				vertices[1].u = 1.0f; vertices[1].v = 1.0f;vertices[1].x =_position.x + _size.x	;vertices[1].y =_position.y				;vertices[1].z =_position.z;
				vertices[2].u = 0.0f; vertices[2].v = 0.0f;vertices[2].x =_position.x			;vertices[2].y =_position.y + _size.y	;vertices[2].z =_position.z;
				vertices[3].u = 1.0f; vertices[3].v = 0.0f;vertices[3].x =_position.x + _size.x	;vertices[3].y =_position.y + _size.y	;vertices[3].z =_position.z;

				RenderManager::Instance()->bindTexture(_back);			
				RenderManager::Instance()->DrawVertexObject(vertices,4,true,Textured,TriangleStrip);
			}			

			//front
			{
				TexturedVertex* vertices = (TexturedVertex*)RenderManager::Instance()->CreateVertexObject(Textured,4);

				vertices[0].u = 1.0f; vertices[0].v = 1.0f;vertices[0].x =_position.x			;vertices[0].y =_position.y				;vertices[0].z =_position.z + _size.z;
				vertices[1].u = 1.0f; vertices[1].v = 0.0f;vertices[1].x =_position.x			;vertices[1].y =_position.y	+ _size.y	;vertices[1].z =_position.z + _size.z;
				vertices[2].u = 0.0f; vertices[2].v = 1.0f;vertices[2].x =_position.x + _size.x	;vertices[2].y =_position.y				;vertices[2].z =_position.z + _size.z;
				vertices[3].u = 0.0f; vertices[3].v = 0.0f;vertices[3].x =_position.x + _size.x	;vertices[3].y =_position.y + _size.y	;vertices[3].z =_position.z + _size.z;

				RenderManager::Instance()->bindTexture(_front);			
				RenderManager::Instance()->DrawVertexObject(vertices,4,true,Textured,TriangleStrip);
			}

			//down
			{
				TexturedVertex* vertices = (TexturedVertex*)RenderManager::Instance()->CreateVertexObject(Textured,4);

				vertices[0].u = 1.0f; vertices[0].v = 1.0f;vertices[0].x =_position.x 			;vertices[0].y =_position.y			;vertices[0].z =_position.z;
				vertices[1].u = 1.0f; vertices[1].v = 0.0f;vertices[1].x =_position.x			;vertices[1].y =_position.y			;vertices[1].z =_position.z + _size.z;
				vertices[2].u = 0.0f; vertices[2].v = 1.0f;vertices[2].x =_position.x + _size.x	;vertices[2].y =_position.y			;vertices[2].z =_position.z;
				vertices[3].u = 0.0f; vertices[3].v = 0.0f;vertices[3].x =_position.x + _size.x	;vertices[3].y =_position.y			;vertices[3].z =_position.z + _size.z;

				RenderManager::Instance()->bindTexture(_down);			
				RenderManager::Instance()->DrawVertexObject(vertices,4,true,Textured,TriangleStrip);
			}

			//up
			{
				TexturedVertex* vertices = (TexturedVertex*)RenderManager::Instance()->CreateVertexObject(Textured,4);

				vertices[0].u = 1.0f; vertices[0].v = 0.0f;vertices[0].x =_position.x 			;vertices[0].y =_position.y	+ _size.y	;vertices[0].z =_position.z;
				vertices[1].u = 0.0f; vertices[1].v = 0.0f;vertices[1].x =_position.x + _size.x	;vertices[1].y =_position.y	+ _size.y	;vertices[1].z =_position.z;
				vertices[2].u = 1.0f; vertices[2].v = 1.0f;vertices[2].x =_position.x			;vertices[2].y =_position.y	+ _size.y	;vertices[2].z =_position.z + _size.z;
				vertices[3].u = 0.0f; vertices[3].v = 1.0f;vertices[3].x =_position.x + _size.x	;vertices[3].y =_position.y	+ _size.y	;vertices[3].z =_position.z + _size.z;

				RenderManager::Instance()->bindTexture(_up);			
				RenderManager::Instance()->DrawVertexObject(vertices,4,true,Textured,TriangleStrip);
			}

			//right
			{
				TexturedVertex* vertices = (TexturedVertex*)RenderManager::Instance()->CreateVertexObject(Textured,4);

				vertices[0].u = 1.0f; vertices[0].v = 1.0f;vertices[0].x =_position.x 		;vertices[0].y =_position.y				;vertices[0].z =_position.z;
				vertices[1].u = 1.0f; vertices[1].v = 0.0f;vertices[1].x =_position.x 		;vertices[1].y =_position.y	+ _size.y	;vertices[1].z =_position.z;
				vertices[2].u = 0.0f; vertices[2].v = 1.0f;vertices[2].x =_position.x		;vertices[2].y =_position.y				;vertices[2].z =_position.z + _size.z;
				vertices[3].u = 0.0f; vertices[3].v = 0.0f;vertices[3].x =_position.x 		;vertices[3].y =_position.y	+ _size.y	;vertices[3].z =_position.z + _size.z;

				RenderManager::Instance()->bindTexture(_right);			
				RenderManager::Instance()->DrawVertexObject(vertices,4,true,Textured,TriangleStrip);
			}

			//left
			{
				TexturedVertex* vertices = (TexturedVertex*)RenderManager::Instance()->CreateVertexObject(Textured,4);

				vertices[0].u = 0.0f; vertices[0].v = 1.0f;vertices[0].x =_position.x + _size.x		;vertices[0].y =_position.y				;vertices[0].z =_position.z;
				vertices[1].u = 1.0f; vertices[1].v = 1.0f;vertices[1].x =_position.x + _size.x		;vertices[1].y =_position.y				;vertices[1].z =_position.z + _size.z;
				vertices[2].u = 0.0f; vertices[2].v = 0.0f;vertices[2].x =_position.x + _size.x		;vertices[2].y =_position.y	+ _size.y	;vertices[2].z =_position.z;
				vertices[3].u = 1.0f; vertices[3].v = 0.0f;vertices[3].x =_position.x + _size.x		;vertices[3].y =_position.y	+ _size.y	;vertices[3].z =_position.z + _size.z;

				RenderManager::Instance()->bindTexture(_left);			
				RenderManager::Instance()->DrawVertexObject(vertices,4,true,Textured,TriangleStrip);
			}

			RenderManager::Instance()->SetDepthTest(true);
			RenderManager::Instance()->SetDepthMask(true);
		}
	}
}
