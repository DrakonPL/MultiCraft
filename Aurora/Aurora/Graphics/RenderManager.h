#ifndef RenderManager_H
#define RenderManager_H

#include <Aurora/Graphics/Camera.h>
#include <Aurora/Graphics/Image.h>
#include <Aurora/Graphics/Sprite.h>
#include <Aurora/Graphics/SpriteAnimation.h>
#include <Aurora/Graphics/Sprite3D.h>
#include <Aurora/Graphics/RenderTexture.h>
#include <Aurora/Graphics/TrueTypeFont.h>
//#include <Aurora/Graphics/ModelObj.h>

#include <Aurora/Math/Vector3.h>
#include <Aurora/Math/Vector2.h>
#include <Aurora/Math/Plane.h>
#include <Aurora/Math/Frustum.h>
#include <Aurora/Utils/PlatformConfigurator.h>

namespace Aurora
{
	namespace Graphics
	{
		enum VertexType
		{
			Simple,
			Textured,
			TextureColor
		};

		enum VertexPrimitive
		{
			Triangle,
			TriangleStrip,
			TriangleFan
		};

		enum FrontFace
		{
			CW,
			CCW
		};

		class TextureManager;
		class TrueTypeFont;

		class RenderManager
		{
		protected:

			int _width;
			int _height;

			int _viewportX;
			int _viewportY;
			int _viewportWidth;
			int _viewportHeight;

			bool _fullScreen;
			bool _vSync;
			float _pov;

			float _zOtrhoMin;
			float _zOtrhoMax;
			float _zMin;
			float _zMax;

			int _currentTexture;
			Camera *_currentCam;

			FrontFace _frontFace;

			static RenderManager *_renderManager;
		
		public:

			static RenderManager* Instance();

			RenderManager();

			void SetPov(float pov);
			void SetZminMax(float zmin,float zmax);
			void SetOrthoZminMax(float zmin,float zmax);
			void SetSize(int width,int height);
			void SetFulscreen(bool state);
			void SetVSync(bool state);
			void SetVievport(int x,int y,int width,int height);

			void SetCurrentCam(Camera *cam);
			Camera* GetCurrentCam();
			
			inline float GetPov() { return _pov; }
			inline float GetZmin() { return _zMin; }
			inline float GetZmax() { return _zMax; }
			inline int GetWidth() { return _width; }
			inline int GetHeight() { return _height; }
			inline bool GetVsync() { return _vSync; }
			inline bool GetFulscreen() { return _fullScreen;}
			inline FrontFace GetFrontFace() {return _frontFace;}

			friend class TextureManager;
			friend class TrueTypeFont;
			friend class ModelObj;

		public:

			static inline unsigned int RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
			{
				return (r) | (g << 8) | (b << 16) | (a << 24);
			}

		public:

			//all virtual functions that platform renderer must implement
			virtual void Init() = 0;

			virtual void SetOrtho() = 0;
			virtual void SetOrtho(float left,float right,float bottom,float top,float zmin,float zmax) = 0;
			virtual void SetPerspective() = 0;			
			virtual void SetPerspective(float pov,float aspect,float zmin,float zmax) = 0;

			virtual void ClearScreen() = 0;
			virtual void StartFrame() = 0;
			virtual void EndFrame() = 0;

			virtual void bindTexture(Image* image) = 0;
			virtual void bindTexture(std::string filename) = 0;

			//textures
			virtual void CreateTexture(Image* image) = 0;
			virtual void CreateTexture(unsigned char* pixels,int width,int height,unsigned int &texId) = 0;
			virtual void CreateEmptyTexture( Image* image, ImageLocation location ) = 0;
			virtual void UpdateTexture(Image* image) = 0;

			//render to texture
			virtual void CreateRenderTexture(RenderTexture* renderTexture) = 0;
			virtual void DestroyRenderTexture(RenderTexture* renderTexture) = 0;
			virtual void StartRenderToTexture(RenderTexture* renderTexture) = 0;
			virtual void EndRenderToTexture(RenderTexture* renderTexture) = 0;
			virtual void RenderToScreen() = 0;

			//camera
			virtual void UpdateCurrentCamera() = 0;
			virtual void ExtractFrustumPlanes(Frustum *frustum) = 0;
			virtual void ExtractFrustumPlanes(Frustum *frustum,float mvpMatrix[16]) = 0;

			//matrix stuff
			virtual void PushMatrix() = 0;
			virtual void PopMatrix() = 0;
			virtual void SetMatrix(float matrix[16]) = 0;
			virtual void MultMatrix(float matrix[16]) = 0;
			virtual void TranslateMatrix(Vector3 pos) = 0;
			virtual void RotateMatrix(Vector3 rotation) = 0;
			virtual void ScaleMatrix(Vector3 scale) = 0;

			virtual void SetColor(unsigned int col) = 0;
			virtual void SetBlending(bool state) = 0;
			virtual void SetFrontFace(FrontFace face) = 0;

			virtual void* CreateVertexObject(VertexType vertexType,int size) = 0;
			virtual void DrawVertexObject(void* vertexObject,int vertexCound,bool textured,VertexType vertexType,VertexPrimitive vertecPrimitive) = 0;

			//2D drawing
			virtual void DrawImage(Image* image) = 0;
			virtual void DrawImage(Image* image,int posx,int posy) = 0;
			virtual void DrawImage(Image* image,int posx,int posy,int width,int height) = 0;
			
			virtual void DrawSprite(Sprite* sprite) = 0;
			virtual void DrawSprite(Sprite* sprite,float posx,float posy) = 0;

			/*
			//3D drawing
			virtual void drawSprite3D(Sprite3D* sprite) = 0;

			virtual void drawCube(unsigned int color,Math::Vector3 position,Math::Vector3 scale,Math::Vector3 rotation) = 0;
			virtual void DrawCubeTextured(Image* texture,Math::Vector3 position,Math::Vector3 scale,Math::Vector3 rotation) = 0;*/

		};
	}
}

#endif
